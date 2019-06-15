# HTTP Server

Server poskytujúce informácie o systéme pomocou HTTP.

# Popis problému

Za úlohu sme maly vytvoriť server ktorý dokáže odpovedať pomocou HTTP protokolu,
na rôzne informácie o systéme. Server bude počúvať na zadanom porte a podľa url
bude vracať požadovanú odpoveď. Server musí správne spracovávať hlavičky HTTP
a vytvárať správne HTTP odpovede. Typ odpovede môže byť text/plain alebo
application/json, podľa výberu klienta. Server by mal podporovať komunikáciu
zo serverom cez webový prehliadač, program curl a wget.

Url na ktoré ma odpovedať:
```
http://servername:12345/hostname
http://servername:12345/cpu-name
http://servername:12345/load
http://servername:12345/load?refresh=5
```

# Návrh riešenia

Projekt riešim v jazyku C, čo činí prácu zo stringami o niečo náročnejšiu. Vyhľadávanie
a formátovanie stringu je robené cez funkcie knižnice __string.h__ a na generovanie
správnej HTTP odpovede je použitá funkcia __sprintf()__ ktorá je veľmi prospešná pre
formátovanie viacerých typov dát na string. Ak požiadavka nebude obsahovat hlavičku
__Accept__ tak dáta budú vrátené vo formáte text/plain.


Pri zadaní chybnej url alebo chybnej požiadavke vracia server `400 Bad Request`,
pri požiadavke ktorá neni typu GET vraciame `405 Method Not Allowed`,
pri type formátovania ktorý vyžaduje klient a nepodporuje server vraciame `406 Not Acceptable`,
pri komunikácii cez iný štandard ako je HTTP/1.1 vraciame `505 HTTP Version Not Supported`
a pri akejkoľvek inej internej chybe servera vraciame `500 Internal Server Error`.


Server si pre každého nového klienta vygeneruje nový process, ktorý obslúži požiadavku
klienta a udržuje spojenie pokial sa klient neodpojí alebo nevyprší desať sekundová časomiera
od prijímutia posledných dát od klienta.

# Inštalácia a preklad aplikácie

Na preklad projektu je použíty príkaz
```
make build
```
ktorým sa program preloží, neni potreba inštalácie žiadnych pomocných knihovní.

# Spustenie aplikácie

Applikácia sa spúšta príkazom
```
make run port=port_number
```
kde port_number značí číslo portu 0 - 65535,
ak port nebude zadaný tak nastáva chyba.

# Omezenie/rozšírenie

Server dovoluje maximálne 200 pripojených socketov v jednom okamihu a dokáže
pracovať aj s ipv6.

Server dokáže reagovať iba na GET požiadavky, a vracať dáta typu text/plain alebo application/json.
