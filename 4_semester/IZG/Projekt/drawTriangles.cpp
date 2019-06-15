#include <student/gpu.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

#include <vector> // TMP

#define MIN(a,b)((a) < (b) ? a : b)
#define MAX(a,b)((a) > (b) ? a : b)

void doPerspectiveDivisionCopy(Vec4*const ndc,Vec4 const*const p){
  for(int a=0;a<3;++a)
    ndc->data[a] = p->data[a]/p->data[3];
  ndc->data[3] = p->data[3];
}

// Parametry hranovych funkci
struct EdgeParam
{
	int deltaX, deltaY;    // Parametry hranove funkce


	// Konstruktory (C++)
	EdgeParam()
	{}


	EdgeParam(int _deltaX, int _deltaY) : deltaX(_deltaX), deltaY(_deltaY)
	{}


	EdgeParam operator=(const EdgeParam &p)
	{
		deltaX = p.deltaX;
		deltaY = p.deltaY;
		return p;
	}
};

typedef std::vector<EdgeParam> EdgeParams;

typedef std::vector<int> EdgeFncValues;

Vec4 computeFragPositionCopy(Vec4 const&p,uint32_t width,uint32_t height){
  Vec4 res;
  res.data[0] = (p.data[0]*.5f+.5f)*width;
  res.data[1] = (p.data[1]*.5f+.5f)*height;
  res.data[2] = p.data[2];
  res.data[3] = p.data[3];
  return res;
}


void createFragmentShaderData(const Vec4 &pos,GPU*const gpu,
GPUVertexShaderData*const vsd1,GPUVertexShaderData*const vsd2,GPUVertexShaderData*const vsd3)
{
  GPUFragmentShaderData shaderData;
  GPUProgram      const* prg = gpu_getActiveProgram(gpu);

  shaderData.inFragment.gl_FragCoord = pos;

  memcpy(shaderData.inFragment.attributes[0].data,vsd1->outVertex.attributes[0].data,sizeof(Vec4));

  prg->fragmentShader(&shaderData);

  uint32_t const pixelIndex = gpu->framebuffer.width*(int)pos.data[1]+(int)pos.data[0];

  gpu->framebuffer.color[pixelIndex] = shaderData.outFragment.gl_FragColor;
}

void pinedaTriangle(
	const Vec4 &vec1, const Vec4 &vec2, const Vec4 &vec3,GPU*const gpu,
  GPUVertexShaderData*const vsd1,GPUVertexShaderData*const vsd2,GPUVertexShaderData*const vsd3
)
{
  Vec4 v1 = computeFragPositionCopy(vec1,gpu->framebuffer.width,gpu->framebuffer.height);
  Vec4 v2 = computeFragPositionCopy(vec2,gpu->framebuffer.width,gpu->framebuffer.height);
  Vec4 v3 = computeFragPositionCopy(vec3,gpu->framebuffer.width,gpu->framebuffer.height);

  // Nalezeni obalky (minX, maxX), (minY, maxY) trojuhleniku.

	// Hledání minima/maxima ze věšech bodů trojúhelníku.
	int minX = MIN(v1.data[0], MIN(v2.data[0], v3.data[0])),
		maxX = MAX(v1.data[0], MAX(v2.data[0], v3.data[0])),
		minY = MIN(v1.data[1], MIN(v2.data[1], v3.data[1])),
		maxY = MAX(v1.data[1], MAX(v2.data[1], v3.data[1]));

	// Oriznuti obalky (minX, maxX, minY, maxY) trojuhleniku podle rozmeru okna.

	// Omezení minim a maxim v bodě X/Y na rozsah <0, width - 1/height - 1>.
	minX = MIN(MAX(minX, 0), gpu->framebuffer.width - 1);
	maxX = MIN(MAX(maxX, 0), gpu->framebuffer.width - 1);
	minY = MIN(MAX(minY, 0), gpu->framebuffer.height - 1);
	maxY = MIN(MAX(maxY, 0), gpu->framebuffer.height - 1);


	// Spocitani parametru hranove funkce (deltaX, deltaY) pro kazdou hranu.
	// Hodnoty deltaX, deltaY jsou souradnicemi vektoru, ktery ma pocatek
	// v prvnim vrcholu hrany, konec v druhem vrcholu.

	// (deltaX, deltaY) = (x1 - x0, y1 - y0)
	EdgeParams edgeParams(3);
	edgeParams[0] = EdgeParam(v2.data[0] - v1.data[0], v2.data[1] - v1.data[1]); // [v1, v2]
	edgeParams[1] = EdgeParam(v3.data[0] - v2.data[0], v3.data[1] - v2.data[1]); // [v2, v3]
	edgeParams[2] = EdgeParam(v1.data[0] - v3.data[0], v1.data[1] - v3.data[1]); // [v3, v1]


	// Vypocet prvnotni hodnoty hranove funkce.

	// E(x, y) = E(minX, minY) = (minY - y0) * deltaX - (minX - x0) * deltaY
	EdgeFncValues edgeFncValues(3);
	edgeFncValues[0] =
		(minY - v1.data[1]) * edgeParams[0].deltaX
		- (minX - v1.data[0]) * edgeParams[0].deltaY; // v1
	edgeFncValues[1] =
		(minY - v2.data[1]) * edgeParams[1].deltaX
		- (minX - v2.data[0]) * edgeParams[1].deltaY; // v2
	edgeFncValues[2] =
		(minY - v3.data[1]) * edgeParams[2].deltaX
		- (minX - v3.data[0]) * edgeParams[2].deltaY; // v3

	// Vyplnovani: Cyklus pres vsechny body (x, y)
	// v obdelniku (minX, minY), (maxX, maxY).
	// Pro aktualizaci hodnot hranove funkce v bode P (x +/- 1, y) nebo
	// P (x, y +/- 1) vyuzijte hodnoty hranove funkce E (x, y) z bodu P (x, y).
	for (float y = minY + 0.5; y <= maxY - 0.5; y+=1.0f)
	{
		// Uložení hodnoty Ei pro aktuální řádek.
		EdgeFncValues currentEdgeFncValues(edgeFncValues);

		for (float x = minX + 0.5; x <= maxX - 0.5; x+=1.0f)
		{
			// Hodnota pixelu se naství pouze, pokud Ei(x, y) >= 0 pro
			// pro všechny body trojúhelníku.
			if (
				currentEdgeFncValues[0] >= 0
				&& currentEdgeFncValues[1] >= 0
				&& currentEdgeFncValues[2] >= 0
			)
			{
        Vec4 pos;
        pos.data[0] = x;
        pos.data[1] = y;

        createFragmentShaderData(pos,gpu,vsd1,vsd2,vsd3);

			}

			// Aktualizace hodnoty Ei po inkrementaci X.
			// E(x + 1, y) = E(x, y) - deltaY
			currentEdgeFncValues[0] -= edgeParams[0].deltaY; // v1
			currentEdgeFncValues[1] -= edgeParams[1].deltaY; // v2
			currentEdgeFncValues[2] -= edgeParams[2].deltaY; // v3
		}

		// Aktualizace hodnoty Ei po inkrementaci Y.
		// E(x, y + 1) = E(x, y) + deltaX
		edgeFncValues[0] += edgeParams[0].deltaX; // v1
		edgeFncValues[1] += edgeParams[1].deltaX; // v1
		edgeFncValues[2] += edgeParams[2].deltaX; // v3
	}

}

void copyVTAttr(GPU const* gpu,GPUAttribute * att,GPUVertexPullerHead const* head,uint64_t vertexID)
{
  if (!head->enabled)
    return;

  GPUBuffer const* buf = gpu_getBuffer(gpu,head->bufferId);
  uint8_t const* ptr = (uint8_t *)buf->data;
  uint32_t const offset = (uint32_t)head->offset;
  uint32_t const stride = (uint32_t)head->stride;
  uint32_t const size = (uint32_t)head->type;
  memcpy(att->data,ptr + offset + vertexID*stride,size);
}

void vertexPuller(GPUInVertex*const inVertex,GPUVertexPuller const* vao,GPU *gpu,uint32_t index)
{
  uint32_t vertexID = index;

  if (gpu_isBuffer(gpu,vao->indices.bufferId))
  {
    GPUBuffer const* buffer = gpu_getBuffer(gpu,vao->indices.bufferId);

    if (vao->indices.type == UINT8)
    {
      vertexID = (uint32_t)((uint8_t *)buffer->data)[vertexID];
    }
    else if(vao->indices.type == UINT16)
    {
      vertexID = (uint32_t)((uint16_t *) buffer->data)[vertexID];
    }
    else
    {
      vertexID = (uint32_t)((uint32_t *) buffer->data)[vertexID];
    }
  }

  inVertex->gl_VertexID = vertexID;

  for (int i = 0; i < MAX_ATTRIBUTE_SIZE/2; i++)
  {
    copyVTAttr(gpu,inVertex->attributes+i,vao->heads+i,vertexID);
  }
}

/// \addtogroup gpu_side Implementace vykreslovacího řetězce - vykreslování trojúhelníků
/// @{

/**
 * @brief This function should draw trianglesGPU*const gpu
 *
 * @param gpu gpu
 * @param nofVertices number of vertices
 */
void gpu_drawTriangles(GPU *const gpu, uint32_t nofVertices)
{

  /// \todo Naimplementujte vykreslování trojúhelníků.
  /// nofVertices - počet vrcholů
  /// gpu - data na grafické kartě
  /// Vašim úkolem je naimplementovat chování grafické karty.
  /// Úkol je složen:
  /// 1. z implementace Vertex Pulleru
  /// 2. zavolání vertex shaderu pro každý vrchol
  /// 3. rasterizace
  /// 4. zavolání fragment shaderu pro každý fragment
  /// 5. zavolání per fragment operací nad fragmenty (depth test, zápis barvy a hloubky)
  /// Více v připojeném videu.

  // Ziskaj vertex puller
  GPUProgram      const* prg = gpu_getActiveProgram(gpu);
  GPUVertexPuller const* vao = gpu_getActivePuller (gpu);

  GPUVertexShaderData   vd;
  GPUVertexShaderData   vd1;
  GPUVertexShaderData   vd2;


  GPUFragmentShaderData fd;

  vd.uniforms = &prg->uniforms;
  vd1.uniforms = &prg->uniforms;
  vd2.uniforms = &prg->uniforms;

  // Loopni cez kazdy vrchol
  for (uint32_t i = 0; i < nofVertices; i+=3)
  {
    // Zavolaj vertex puller pre kazdy vrchol
    vertexPuller(&vd.inVertex,vao,gpu,i);
    vertexPuller(&vd1.inVertex,vao,gpu,i+1);
    vertexPuller(&vd2.inVertex,vao,gpu,i+2);

    // Zavolaj vertex shader pre kazdy vrchol
    prg->vertexShader(&vd);
    prg->vertexShader(&vd1);
    prg->vertexShader(&vd2);

    Vec4 pos;
    Vec4 pos1;
    Vec4 pos2;

    copy_Vec4(&pos,&vd.outVertex.gl_Position);
    copy_Vec4(&pos1,&vd1.outVertex.gl_Position);
    copy_Vec4(&pos2,&vd2.outVertex.gl_Position);

    Vec4 ndc;
    Vec4 ndc1;
    Vec4 ndc2;

    doPerspectiveDivisionCopy(&ndc,&pos);
    doPerspectiveDivisionCopy(&ndc1,&pos1);
    doPerspectiveDivisionCopy(&ndc2,&pos2);

    pinedaTriangle(ndc,ndc1,ndc2,gpu,&vd,&vd1,&vd2);
  }

  (void)gpu;
  (void)nofVertices;
}

/// @}
