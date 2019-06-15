#include <time.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
  // File holder
	FILE *cpu_stat;

	// Create timespec
	struct timespec sleep_time,time;
	sleep_time.tv_sec = 0;
	sleep_time.tv_nsec = 75000000;

	// Firs cpu variables
	long userPrev = 0;
	long nicePrev = 0;
	long systemPrev = 0;
	long idlePrev = 0;
	long iowaitPrev = 0;
	long irqPrev = 0;
	long softirqPrev = 0;
	long stealPrev = 0;
	long guestPrev = 0;
	long guest_nicePrev = 0;

	// Load file
	if ((cpu_stat = fopen("/proc/stat","rb")) == NULL)
	{
		fprintf(stderr,"Problem with opening file.\n");
		return -1;
	}

	// Get the values from file
	fscanf(cpu_stat,"cpu %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld", &userPrev, &nicePrev, &systemPrev, &iowaitPrev, &idlePrev, &irqPrev, &softirqPrev, &stealPrev, &guestPrev, &guest_nicePrev);

	// Close file
	fclose(cpu_stat);

	// Wait for 75000000 nanoseconds
	nanosleep(&sleep_time,&time);

	// Load second time
	long user = 0;
	long nice = 0;
	long system = 0;
	long idle = 0;
	long iowait = 0;
	long irq = 0;
	long softirq = 0;
	long steal = 0;
	long guest = 0;
	long guest_nice = 0;

	// Load file
	if ((cpu_stat = fopen("/proc/stat","rb")) == NULL)
	{
		fprintf(stderr,"Problem with opening file.\n");
		return -1;
	}

	// Get the values from file
	fscanf(cpu_stat,"cpu %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld", &user, &nice, &system, &iowait ,&idle, &irq, &softirq, &steal, &guest, &guest_nice);

	// Close file
	fclose(cpu_stat);


    printf("%ld <> %ld\n", userPrev,user);
    printf("%ld <> %ld\n", nicePrev,nice);
    printf("%ld <> %ld\n", systemPrev,system);
    printf("%ld <> %ld\n", idlePrev,idle);
    printf("%ld <> %ld\n", iowaitPrev,iowait);
    printf("%ld <> %ld\n", irqPrev,irq);
    printf("%ld <> %ld\n", softirqPrev,softirq);
    printf("%ld <> %ld\n", stealPrev,steal);
    printf("%ld <> %ld\n", guestPrev,guest);
    printf("%ld <> %ld\n", guest_nicePrev,guest_nice);



    	// Stackoverflow calculation
    	double PrevIdle = idlePrev + iowaitPrev;
    	double Idle = idle + iowait;
    	double PrevNonIdle = userPrev + nicePrev + systemPrev + irqPrev + softirqPrev + stealPrev;
    	double NonIdle = user + nice + system + irq + softirq + steal;
    	double PrevTotal = PrevIdle + PrevNonIdle;
    	double Total = Idle + NonIdle;
    	double totald = Total - PrevTotal;
    	double idled = Idle - PrevIdle;
    	double CPU_Percentage = 0;

    	// To protect from division by zero
    	if (totald != 0)
    	{
    		CPU_Percentage = (totald - idled)/totald * 100;
    	}

    	// Round to integer
    	int cpu_usage = (int)(CPU_Percentage + 0.5);

      printf("CPU: %d\n", cpu_usage);

  return 0;
}
