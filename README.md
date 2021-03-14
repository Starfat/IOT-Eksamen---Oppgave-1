# IOT-Eksamen Oppgave-1
WasteScan

**Prosjektets målsetning**

Målet er å gjøre det lettere å sortere avfall for gjenvinning, og potensielt unngå å sortere avfall. Det
kan også motivere mennesker og føre til mer og bedre kildesortering. Dette kan spesielt være tilfelle
for offentlige søppelkasser og større lokaler som USN. Ideen er å veilede hvilken søppelkasse
brukeren skal bruke. Løsningen vil gi forbrukerne veiledning om avfallssortering på
gjenvinningsstasjoner rundt for eksempel på Campus Vestfold.

**Kort om løsningen**

Prototypen består av en mobil enhet med en app som skanner strekkoder. I tillegg vil det være montert
en IoT-enhet på gjenvinningsstasjonen. Denne IoT-enheten vil, gjennom et LCD-display, vise hvilken
avfallskategori et element du vil resirkulere er i. Løsningen vil logge bruken og vil kunne trekke ut
statistikk. Se attchemnet for et systemskjema. (att, 1)
Strekkoder registreres i en database av kategoriene plast, papir, restavfall, glass og metall og bio. Ved
hjelp av mobilappen "Barcode Scanner Terminal" kan du bruke et mobilkamera til å skanne en
strekkode. Appen gjenkjenner strekkodenummeret og gjør det til en tekststreng. Denne strengen
sendes til en skytjeneste, som sjekker strekkodenummeret mot databasen og returnerer informasjon om
hvilken kategori strekkoden tilhører. Denne informasjonen vises i både mobilappen og LCD-skjermen

**Maskinvare / Teknisk løsning**
- Mobiltelefon med android 4.0 eller høyere
- Barcode Skanner Terminal for Android
- NodeMCU microcontroller med LCD-skjerm
- Microsoft Azure SQL Database
- Microsoft Azure Logic App
- Microsoft Azure IoT Hub
- Enhetene kommuniserer via HTTP POST og GET Requests
