// CYD Trivia Game
// Display: nur Frage (gross), Spieler antworten per Handy
// Libraries: TFT_eSPI, XPT2046_Touchscreen, WiFi, WebServer, DNSServer

#include "FS.h"
#include "SPI.h"
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

TFT_eSPI tft = TFT_eSPI();

// ── Touch ────────────────────────────────────────────────────────────────────
#define XPT2046_IRQ  36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK  25
#define XPT2046_CS   33
SPIClass touchSPI = SPIClass();
XPT2046_Touchscreen ts(XPT2046_CS, XPT2046_IRQ);

// ── Netzwerk ─────────────────────────────────────────────────────────────────
#define AP_SSID   "CYD-Trivia"
#define AP_IP_STR "192.168.4.1"
WebServer server(80);
DNSServer dns;

// ── Display ──────────────────────────────────────────────────────────────────
#define SCREEN_W  320
#define SCREEN_H  240
#define COL_BG    0x0841
#define COL_HEAD  0x8010
#define COL_CARD  0x18C3
#define COL_TEXT  0xFFFF
#define COL_MUTED 0x8410
#define COL_GREEN 0x07E0
#define COL_RED   0xF800
#define COL_BLUE  0x035F
#define COL_YELL  0xFFE0
#define COL_PURP  0x780F
#define COL_ORNG  0xFD00

#define NUM_QUESTIONS_PER_GAME 10

struct Question {
  const char* text;
  const char* answers[4];
  uint8_t correct;
};

#define NUM_CATEGORIES 13

enum Category {
  CAT_ALL = 0,
  CAT_UNTERHALTUNG = 1,
  CAT_WISSENSCHAFT = 2,
  CAT_WISSENSCHAFTNATUR = 3,
  CAT_GESCHICHTE = 4,
  CAT_GEOGRAPHIE = 5,
  CAT_FAHRZEUGE = 6,
  CAT_SPORT = 7,
  CAT_PROMINENTE = 8,
  CAT_TIERE = 9,
  CAT_ALLGEMEINWISSEN = 10,
  CAT_MYTHOLOGIE = 11,
  CAT_POLITIK = 12,
};

const char* CATEGORY_NAMES[NUM_CATEGORIES] = {
  "Alle", "Unterhaltung", "Wissenschaft", "Wiss. & Natur", "Geschichte", "Geographie", "Fahrzeuge", "Sport", "Prominente", "Tiere", "Allgemeinwissen", "Mythologie", "Politik"
};

#define NUM_Q_UNTERHALTUNG 50
const Question questions_Unterhaltung[NUM_Q_UNTERHALTUNG] = {
  {"In & quot; es ist immer sonnig in Philadelphia & quot;Wie hiess Franks Wrestling -Persona?", {"Kriegsvogel", "Der Verrueckte", "Tag Mann", "Der Muellmann"}, 3},
  {"Wer schrieb das \"ein Lied aus Eis und Feuer\" \"Fantasy -Roman -Serie?", {"George Eliot", "George Orwell", "George Lucas", "George R. R. Martin"}, 3},
  {"Welches System war \"touhou: sehr reaktionsschnell auf Gebete & quot;veroeffentlicht auf?", {"Windows 98", "OSX 10.9.5", "Fenster mich", "PC-98"}, 3},
  {"Welcher Touhou -Projektcharakter war der erste Auftritt als Midboss im achten Spiel, unvergaengliche Nacht?", {"Mystia Lorelei", "Rumia", "Kaguya Houraisan", "Tewi Inaba"}, 3},
  {"Wann war & quot; luigis Herrenhaus 3 & quot;freigegeben?", {"6. September 2018", "1. Oktober 2019", "13. Januar 2019", "31. Oktober 2019"}, 3},
  {"Wie hiess der von Blizzard Entertainment projizierte abgesagt, der spaeter zu Overwatch werden wuerde?", {"Omega", "Geist", "Omnic", "Titan"}, 3},
  {"Wie heisst in Game of Thrones, der richtige Name?", {"Lancel Lannister", "Torrhen Karstark", "Petyr Baelish", "Podrick Payne"}, 2},
  {"Gordon Freeman soll gebrannt und zerstoert haben, welche Nahrung in der Pausenraummikrowelle?", {"Huehnersuppe", "Sub -Sandwich", "Peperoni Pizza", "Kasserolle"}, 3},
  {"In den Skylanders Giants war Zaps?Wow pow genannt \"fuer Riley & quot;?", {"Nach einem Sprachschauspieler", "Als Verweis auf einen Film", "Als Hommage an einen Fan, der verstorben ist", "Nach einem der Autoren des Spiels"}, 2},
  {"Wie oft kaempfen Sie gegen die Inhaftierten in der Legende von Zelda: Skyward Sword?", {"3", "5", "2", "4"}, 0},
  {"Was haben die Videospiele, die NO MANN HIME UND MISTIGE Nr. 9 gemeinsam haben?", {"Beide wurden 2013 bekannt gegeben.", "Beide wurden von Indie Studios entwickelt.", "Beide wurden fuer die PlayStation 3 veroeffentlicht.", "Beide wurden crowdfunded."}, 0},
  {"Einer der fruehen Prototypen des Sega Dreamcast -Controllers aehnelte der der folgenden?", {"Foen", "Fernseh -Fernbedienung", "Tablette", "Flip -Telefon"}, 1},
  {"Was ist Pikachus National Pok & eacute; Dex -Nummer?", {"#109", "#025", "#031", "#001"}, 1},
  {"In welchem Unternehmen hat Gabe Newell vor der Gruendung Valve Corporation gearbeitet?", {"Yahoo", "Microsoft", "Apfel", "Google"}, 1},
  {"Welches Animationsstudio hat \"Gurren Lagann\" produziert.", {"Gainax", "Pierrot", "A-1 Bilder", "Kyoto -Animation"}, 0},
  {"Was ist die Boss -Runde in der \"Call of Duty: Zombies\" & quot;Karte \"fuenf\"?", {"Napalm Zombie", "Hoellenhunde", "Der Pentagon -Dieb", "Sprung von Jacks"}, 2},
  {"Wer ist der Saenger und Frontmann der Rockband & Quot; Guns N 'Rosen \"?", {"Schraegstrich", "Bono", "Axl Rose", "Kurt Cobain"}, 2},
  {"Was ist der Untertitel fuer Gran Turismo 3?", {"A-Spec", "Nitro", "Fahren", "Meisterschaft"}, 0},
  {"In & quot; Call of Duty: Zombies & quot;, welche Karte enthaelt die \"Fly -Trap\" & quot;Osterei?", {"Tranzit", "Der Riese", "Ruf der Toten", "Shi no numa"}, 1},
  {"Wer ist der Hauptprotagonist von \"Ace Combat Zero: The Belkan War\"?", {"Pixy", "Chiffre", "Mobius 1", "Brand"}, 1},
  {"In den meisten FPS-Videospielen wie Counter-Strike schiesst das Schiessen, welcher Koerperteil am hoechsten Schaden anrichtet?", {"Brust", "Kopf", "Bein", "Arm"}, 1},
  {"Welches dieser Chefs sind im Spiel \"Terraria\" vor der Hardmode-Chefs?", {"Skeletron Prime", "Auge von Cthulhu", "Plantera", "Der Zerstoerer"}, 1},
  {"Wer war der Sprachschauspieler fuer Schlangen in Metal Gear Solid V: Der Phantomschmerz?", {"Norman Reedus", "David Hayter", "Hideo Kojima", "Kiefer Sutherland"}, 3},
  {"In der \"Pikmin\" & quot;Spiele, welche der folgenden Pikmin -Farben fehlt es, wenn es sich um eine eigene \"Onion\" -Erion handelt.Nest?", {"Felsen", "Gefluegelt", "Blau", "Lila"}, 3},
  {"In der Show & quot; Dragonball z & quot;, wie heisst der maechtigste Angriff von Zellen?", {"Cell Kamehameha", "Android Kamehameha", "Super Kamehameha", "Solar Kamehameha"}, 3},
  {"Wann wurde das Brettspiel Twister fuer die Oeffentlichkeit veroeffentlicht?", {"September 1965", "April 1966", "Januar 1969", "Februar 1966"}, 1},
  {"Wer spricht den Charakter \"Reigen\"?Im englischen Dub of & quot; Mob Psycho 100 & quot;?", {"Max Mittelman", "Chris Niosi", "Kyle McCarley", "Casey Mongillo"}, 1},
  {"Wie heisst in der TF2 -Ueberlieferung die juengeren Schwestern der Schweren?", {"Anna und Bronislava", "Yanna und Gaba", "Yana und Bronislava", "Gaba und Anna"}, 2},
  {"In der Magie: Das Versammlungsuniversum, die Antiquitaeten, die Eiszeit und die Expansionen der Allianzen finden auf welchem Kontinent statt?", {"Jamuraa", "Shiv", "Terisiare", "Luftfahrt"}, 2},
  {"Wie heisst der Attentaeter im ersten \"Hellboy\".Film?", {"Ilsa Haupstein", "Klaus Werner von Krupt", "Grigori Efimovich Rasputin", "Karl RUPRTCHT KROENEN"}, 3},
  {"Welche der folgenden Aussagen ist im Original Doom (1993) kein Cheat -Code?", {"Iddqd", "IDFA", "Idclip", "Idspispopd"}, 2},
  {"In der Show & quot; futurama & quot;Wie heisst der vollstaendige Name des Fry?", {"Fry Rodr & iacute; Guez", "Philip J. Fry", "Philip braten", "Fry J. Philip"}, 1},
  {"Wie ist die Beziehung zwischen den Bandmitgliedern der American Rock Band King of Leon?", {"Brueder & amp;Cousins", "Freunde aus Kindertagen", "Bruederlichkeitshausmitglieder", "Ehemalige Klassenkameraden"}, 0},
  {"Welcher dieser Songs ist nicht in den Beatles 'Album & quot; Sgt.Pepper ' s Lonely Hearts Club Band & quot;?", {"Besser werden", "Ein Loch reparieren", "Lucy am Himmel mit Diamanten", "Erdbeerfelder fuer immer"}, 3},
  {"Wie viel kostet der ' AWP 'Kosten in Counter-Strike: Global Offensive?", {"$ 4650", "$ 5000", "$ 4500", "$ 4750"}, 3},
  {"Welche der folgenden Aussagen ist eine Klasse im Spiel \"Hearthstone\"?", {"Kleriker", "Moench", "Priester", "Salbei"}, 2},
  {"Wie heisst die Hauptfigur aus dem Musikvideo von \"Shelter\".Von Porter Robinson und A-1 Studios?", {"Rin", "Rem", "RAM", "Ren"}, 0},
  {"Wer hat das Videospiel 2011 \"Minecraft\" erstellt?", {"Carl Manneh", "Daniel Rosenfeld", "Markus Persson", "Jens Bergensten"}, 2},
  {"In welchem Land befindet sich Cory im Haus?", {"Kanada", "England", "Die Vereinigten Staaten von Amerika", "Venezuela"}, 2},
  {"In dem britischen Film von 1979 \"Quadrophenia\" & quot;Wie heisst die Kuestenstadt, die die Mods besuchen?", {"Bridlington", "Eastbourne", "Brighton", "Mausloch"}, 2},
  {"Was war das erste Spiel im \"Battlefield\" & quot;Serie?", {"Schlachtfeld 2", "Battlefield Vietnam", "Battlefield 1942", "Schlachtfeld 1"}, 2},
  {"Nach \"Star Wars\" & quot;Ueberlieferung, aus welchem Planet kommt Obi-Wan Kenobi?", {"Tatooine", "Naboo", "Alderaan", "Stewjon"}, 3},
  {"Wo finden sich Terrorfidens haeufiger im Nintendo -Spiel Miitopia?", {"Anderswelt", "Der Wolkenkratzer", "Pekulia", "Neue Lumos"}, 3},
  {"Welches Rennen geniesst ein Glas warmes Baghol in \"Star Trek\"?", {"Menschlich", "Klingon", "Botha", "Vulkan"}, 1},
  {"Was ist der Name von Batmans Eltern?", {"Todd & amp;Mira", "Jason & amp;Sarah", "Joey & amp;Jackie", "Thomas & amp;Martha"}, 3},
  {"Welcher Pok & eacute; Mons Basis -STAT -Gesamtzahl aendert sich nicht, wenn es sich weiterentwickelt?", {"Schnaeppchen", "Pikachu", "Larven", "Scyther"}, 3},
  {"Wer leitete die Filme \"Pulp Fiction\", \"Reservoir Dogs\" & quot;und \"Django Unchained\"?", {"Quentin Tarantino", "James Cameron", "Martin Scorsese", "Steven Spielberg"}, 0},
  {"Im \"Jurassic Park\" & quot;Universum, was war der erste Dinosaurier, der 1986 von Ingen klonte?", {"Triceratops", "Brachiosaurus", "Velociraptor", "Troodon"}, 2},
  {"In Magic: The Gathering, welche der folgenden folgenden, war die erste oeffentlich veroeffentlichte Folienkarte?", {"Revenant", "Blitzdrache", "Lasttier", "Dirtcowl Wurm"}, 1},
  {"Wie viele Jahreszeiten hat die TV -Show \"Donkey Kong Country\" angegeben?zuletzt?", {"2", "5", "1", "4"}, 0}
};

#define NUM_Q_WISSENSCHAFT 44
const Question questions_Wissenschaft[NUM_Q_WISSENSCHAFT] = {
  {"Wo befindet sich der Gluteus Maximus Muskel?", {"Kopf", "Arm", "Torso", "Hintern"}, 3},
  {"Wie heisst Layer 4 des ISO -Modells (Open Systems Interconnection)?", {"Datenverbindung", "Transport", "Sitzung", "Netzwerk"}, 1},
  {"Wer hat einen Dampfmotor patentiert, der eine kontinuierliche Drehbewegung erzeugt hat?", {"James Watt", "Albert Einstein", "Alessandro Volta", "Nikola Tesla"}, 0},
  {"Wie viel Speicherplatz hat beim ersten Start Google Mail Ihre E -Mail bereitgestellt?", {"1 GB", "512MB", "5 GB", "Unbegrenzt"}, 0},
  {"Welcher dieser Namen war ein tatsaechlicher Codename fuer ein storniertes Microsoft -Projekt?", {"Enceladus", "Neptun", "Saturn", "Pollux"}, 1},
  {"Die Programmiersprache ' Swift 'wurde erstellt, um die andere Programmiersprache zu ersetzen?", {"C#", "Ziel-C", "C ++", "Rubin"}, 1},
  {"In der Server -Hosting -Branche steht IaaS fuer ...", {"Infrastruktur als Server", "Infrastruktur als Service", "Internet als Service", "Internet und ein Server"}, 1},
  {"Was ist die Quadratwurzel von 49?", {"4", "7", "9", "12"}, 1},
  {"Welches davon ist der Name fuer das von der National Security Agency 1993 eingefuehrte fehlgeschlagene Treuhandel?", {"Clipper -Chip", "Enigma -Maschine", "Nautilus", "Skipjack"}, 0},
  {"Was ist das Symbol fuer die Verschiebung?", {"Dp", "& Delta; r", "R", "Dr."}, 1},
  {"Welche der folgenden Sprachen wird als Skriptsprache in der Unity 3D Game Engine verwendet?", {"Java", "Ziel-C", "C ++", "C#"}, 3},
  {"Wofuer steht LTS auf dem Softwaremarkt?", {"Langfristige Unterstuetzung", "Laecherliche Turbogeschwindigkeit", "Laecherliche Uebertragungsgeschwindigkeit", "Langer Taco -Service"}, 0},
  {"Was war das erste Unternehmen, das den Begriff \"Golden Master\" verwendet hat.", {"Apfel", "IBM", "Google", "Microsoft"}, 0},
  {"Dieses mobile Betriebssystem hatte 2012 den groessten Marktanteil.", {"Brombeere", "Android", "Symbian", "iOS"}, 3},
  {"Welche Codierungssprache war die Nr. 1 Programmiersprache in Bezug auf die Verwendung von GitHub im Jahr 2015?", {"Python", "C#", "JavaScript", "Php"}, 2},
  {"Welche davon ist keine Ebene im OSI -Modell fuer die Datenkommunikation?", {"Transportschicht", "Physische Schicht", "Verbindungsschicht", "Anwendungsschicht"}, 2},
  {"Wie heisst Android 4.3?", {"Lutscher", "Froyo", "Nutella", "Geleebohne"}, 3},
  {"Die in vielen 3D -Modellierungsanwendungen oft gesehene Teekanne heisst was?", {"3D -Teekanne", "Utah -Teekanne", "Tennessee Teekanne", "Pixar -Teekanne"}, 1},
  {"Mobile Hardware- und Softwareunternehmen & quot; Blackberry Limited & quot;wurde in welchem Land gegruendet?", {"Vereinigte Staaten von Amerika", "Norwegen", "Kanada", "Vereinigtes Koenigreich"}, 2},
  {"Welcher virtuelle Assistent wird von Amazon entwickelt?", {"Google Assistant", "Alexa", "Siri", "Cortana"}, 1},
  {"Welches der folgenden Mobilfunkunternehmen hat in Asien nicht ihren Hauptsitz?", {"LG -Elektronik", "HTC", "Samsung", "Nokia"}, 3},
  {"Welche Menge an Bits ist ueblicherweise einem Byte?", {"2", "1", "8", "64"}, 2},
  {"Welche Programmiersprache teilt ihren Namen mit einer Insel in Indonesien?", {"C", "Python", "Jakarta", "Java"}, 3},
  {"Welche dieser mechanischen Tastaturschalter von Cherry MX ist sowohl taktil als auch klickend?", {"Cherry Mx rot", "Cherry Mx Blue", "Cherry Mx schwarz", "Cherry Mx Brown"}, 1},
  {"Wie viele Zeptometres befinden sich in einem Femtometer?", {"1.000.000", "10", "1000", "1.000.000.000"}, 0},
  {"Wie viele Nullen gibt es in einem Googol?", {"10", "1.000", "100", "1.000.000"}, 2},
  {"Was ist der richtige Begriff fuer das Metallobjekt zwischen der CPU und dem CPU -Luefter innerhalb eines Computersystems?", {"Waermeentlueftung", "Temperaturdecipator", "Kuehlkoerper", "CPU -Ablagerung"}, 2},
  {"Wie viele Werte kann ein einzelnes Byte darstellen?", {"8", "1024", "1", "256"}, 3},
  {"Welcher griechische Mathematiker lief nackt durch die Strassen von Syrakus, waehrend er \"Eureka\" rief;Nach dem Entdecken des Prinzips der Verschiebung?", {"Eratosthenes", "Euklid", "Homer", "Archimedes"}, 3},
  {"Welche der folgenden Informationen ist die aelteste dieser Computer bis zum Veroeffentlichungsdatum?", {"Commodore 64", "ZX -Spektrum", "Apfel 3", "TRS-80"}, 3},
  {"Was entspricht XL in roemischen Ziffern?", {"40", "60", "15", "90"}, 0},
  {"Wie werden die Basisstation Tracker fuer den HTC Vive genannt?", {"Bewegung", "Leuchtturm", "Konstellation", "Tracker"}, 1},
  {"Welches Unternehmen hat die HoloLens entwickelt?", {"Microsoft", "HTC", "Oculus", "Tobii"}, 0},
  {"Wie hiess die Sicherheitsanfaelligkeit in Bash im Jahr 2014?", {"Bashbug", "StageFright", "Shellshock", "Heartbleed"}, 2},
  {"Welche der folgenden Computerkomponenten kann nur mit NAND -Toren erstellt werden?", {"RAM", "CPU", "Alu", "Registrieren"}, 2},
  {"Laserjet- und Tintenstrahldrucker sind beide Beispiele fuer die Art von Drucker?", {"Punktmatrixdrucker", "Daisywheel -Drucker", "Nicht-Impact-Drucker", "Aufpralldrucker"}, 2},
  {"Wofuer steht Midi im Computer?", {"Datenschnittstelle fuer Musikinstrumente", "Modulare Schnittstelle digitaler Instrumente", "Musikalische Schnittstelle digitaler Instrumente", "Digitale Schnittstelle fuer Musikinstrumente"}, 3},
  {"In der Programmiersprache \"Python\", welche dieser Aussagen wuerde den String \"Hello World\" anzeigen?korrekt?", {"print (\"Hallo Welt\")", "printf (\"Hallo Welt\")", "console.log (\"Hallo Welt\")", "Echo & quot; Hallo Welt & quot;"}, 0},
  {"Der Begriff von A & quot; Set, der alle Saetze enthaelt, die sich nicht selbst enthalten;Wird eine paradoxe Idee welcher englischen Philosphaere zugeschrieben?", {"Francis Bacon", "John Locke", "Bertrand Russell", "Alfred North Whitehead"}, 2},
  {"Welche Firma wurde am 1. April 1976 von Steve Jobs, Steve Wozniak und Ronald Wayne gegruendet?", {"Kommodore", "Apfel", "Atari", "Microsoft"}, 1},
  {"Wie heisst das Standardthema, das mit Windows XP installiert ist?", {"Luna", "Neptun", "Whistler", "Wonne"}, 0},
  {"Wofuer steht GHZ?", {"Gigahertz", "Gigahatz", "Gigahertz", "Gigahotz"}, 0},
  {"Wie viele Bits bilden den wichtigen Teil einer einzelnen Praezisions -Schwimmpunktzahl?", {"15", "8", "53", "23"}, 3},
  {"Welches Computer -Hardware -Geraet bietet eine Schnittstelle fuer alle anderen angeschlossenen Geraete fuer Kommunikation?", {"Festplattenantrieb", "Zentrale Verarbeitungseinheit", "Hauptplatine", "Zufallszugriffsspeicher"}, 2}
};

#define NUM_Q_WISSENSCHAFTNATUR 44
const Question questions_WissenschaftNatur[NUM_Q_WISSENSCHAFTNATUR] = {
  {"Was wird als die reinste Form der Farbblindheit angesehen?", {"Blau", "Lila", "Rot", "Gruen"}, 0},
  {"Wie viele Isomere hat Butanol (C4H9OH) in der Chemie?", {"6", "5", "4", "3"}, 2},
  {"71% der Oberflaeche der Erde besteht aus", {"Wasser", "Wuesten", "Waelder", "Kontinente"}, 0},
  {"Welches der folgenden Raumfahrzeuge hat den Mond nie beruehrt?", {"Mariner 4", "Smart-1", "Luna 2", "Apollo 11"}, 0},
  {"Was ist die primaere suechtig machende Substanz im Tabak?", {"Nikotin", "Ephedrine", "Glaucin", "Cathinone"}, 0},
  {"Was ist die chemische Formel fuer Ammoniak?", {"NO3", "CH4", "NH3", "CO2"}, 2},
  {"Auf welcher Mission hat sich der Space Shuttle Columbia beim Wiedereintritt getrennt?", {"STS-107", "STS-109", "STS-61-C", "STS-51-l"}, 0},
  {"Welches Edelgas hat die niedrigste Atomzahl?", {"Krypton", "Neon", "Argon", "Helium"}, 3},
  {"Welche dieser Fruechte ist botanisch gesehen keine Beere?", {"Blaubeere", "Concord Traube", "Erdbeere", "Banane"}, 2},
  {"Ein Positron ist ein Antipartikel von einem Was?", {"Photon", "Neutron", "Elektron", "Proton"}, 2},
  {"Wer hat die Entdeckung von Roentgenaufnahmen gemacht?", {"Thomas Alva Edison", "James Watt", "Albert Einstein", "Wilhelm Conrad r & oUml; ntgen"}, 3},
  {"Wie lautet der offizielle Name des Sterns, der dem North Celestial Pole am naechsten liegt?", {"Eridanus", "Polaris", "Gamma Cephei", "Iota Cephei"}, 1},
  {"Welche der folgenden Aussagen ist kein echtes Element?", {"Praseodym", "Hsium", "Vitrainium", "Lutetium"}, 2},
  {"Folsaeure ist die synthetische Form von welchem Vitamin?", {"Vitamin c", "Vitamin b", "Vitamin d", "Vitamin a"}, 1},
  {"Menschliche Zellen haben typischerweise wie viele Kopien jedes Gens?", {"2", "4", "3", "1"}, 0},
  {"Welcher Mond ist der einzige Satellit in unserem Sonnensystem, der eine dichte Atmosphaere besitzt?", {"Europa", "Titan", "Miranda", "Callisto"}, 1},
  {"Welches Element hat die Atomzahl von 7?", {"Sauerstoff", "Neon", "Stickstoff", "Wasserstoff"}, 2},
  {"Die \"Tibia\" & quot;Wird in welchem Koerperteil gefunden?", {"Arm", "Bein", "Kopf", "Hand"}, 1},
  {"Wo befindet sich der Gluteus Maximus -Muskel?", {"Hintern", "Torso", "Kopf", "Arm"}, 0},
  {"AU auf der Periodenzuechter bezieht sich auf welches Element?", {"Gold", "Silber", "Sauerstoff", "Nickel"}, 0},
  {"Wie lange dauert ungefaehr ein Jahr auf Uranus?", {"84 Erdjahre", "47 Erdjahre", "109 Erdjahre", "62 Erdjahre"}, 0},
  {"Was ist die molekulare Formel der aktiven Komponente von Chilischoten (Capsaicin)?", {"C6H4CL2", "C18H27NO3", "C21H23NO3", "C13H25NO4"}, 1},
  {"Welche der folgenden Aussagen gilt als klassische Konditionierung?", {"Skinner Box Experiment", "Schr & OUML; Dinger -Katzenexperiment", "Harlow & rsquos Affenexperimente", "Pavlovs Hundeversuche"}, 3},
  {"Welcher dieser Wissenschaftler fuehrte das Goldfolienexperiment durch, das zu dem Schluss kam, dass Atome hauptsaechlich aus leerem Raum bestehen?", {"Niels Henrik David Bohr", "Joseph John Thomson", "Ernest Rutherford", "Archimedes"}, 2},
  {"Was repraesentiert der gelbe Diamant auf dem NFPA 704 Fire Diamond?", {"Reaktivitaet", "Gesundheit", "Entflammbarkeit", "Radioaktivitaet"}, 0},
  {"Das Wort & quot; Science & quot;stammt aus dem Wort & quot; Scire & quot;Das heisst was?", {"Zu wissen", "Zu messen", "Zaehlen", "Leben"}, 0},
  {"In der Physik sind die Energieerhaltung und die Erhaltung des Impulses beide Folgen von welcher der folgenden?", {"Wick ' s Theorem", "NOETHER \"STEOREM", "Carnots Theorem", "Glockensatz"}, 1},
  {"Wer hat in den 1950er Jahren den ersten erfolgreichen Polio -Impfstoff entwickelt?", {"John F. Enders", "Thomas Weller", "Frederick Robbins", "Jonas Salk"}, 3},
  {"Der Asteroidenguertel befindet sich zwischen den beiden Planeten?", {"Erde und Mars", "Quecksilber und Venus", "Mars und Jupiter", "Jupiter und Saturn"}, 2},
  {"Welcher der folgenden Kunststoff wird ueblicherweise fuer Fensterrahmen, Dachrinnen und Abflussrohre verwendet?", {"Polypropylen (PP)", "Polyvinylchlorid (PVC)", "Polystyrol (PS)", "Polyethylen (PE)"}, 1},
  {"Welcher der folgenden Aussagen ist der Begriff fuer \"chirurgische Komplikationen\", die sich aus chirurgischen Schwaemmen ergeben, die sich im Koerper des Patienten befinden?", {"Meupareunia", "Gossypiboma", "Jentakulaer", "Gongoozler"}, 1},
  {"Welches Mineral hat die niedrigste Zahl auf der MOHS -Skala?", {"Gips", "Talk", "Quarz", "Diamant"}, 1},
  {"Was ist das gleiche in Celsius und Fahrenheit?", {"-40", "-42", "32", "-39"}, 0},
  {"Was ist Isobutylphenylpropansaeure allgemein bekannt?", {"Ketamin", "Aspirin", "Morphium", "Ibuprofen"}, 3},
  {"Wie viele Planeten befinden sich in unserem Sonnensystem?", {"Acht", "Sieben", "Zehn", "Neun"}, 0},
  {"Wie viele Grad Fahrenheit sind 100 Grad Celsius?", {"100", "326", "451", "212"}, 3},
  {"Wofuer steht Laser?", {"Leben Antimaterie durch Stehen der Reichweite", "Lite analysiert von Stereo Ecorazer", "Lichtverstaerkung durch stimulierte Strahlungsemission", "Lichtverstaerker durch Standby -Energie des Radios"}, 2},
  {"Wie heisst der wissenschaftliche Name des Roten Fuchs?", {"Roter Fuchs", "Vulpes Vulpie", "Vulpes Redus", "Vulpes Vulpes"}, 3},
  {"Welches dieser Elemente ist ein Metalloid?", {"Antimon", "Rubidium", "Zinn", "Brom"}, 0},
  {"Was studieren Sie, wenn Sie Entomologie studieren?", {"das Gehirn", "Insekten", "Menschen", "Fisch"}, 1},
  {"Wann wurde das erste Saeugetier erfolgreich geklont?", {"1996", "1985", "2009", "1999"}, 0},
  {"Muskelfasern besteht aus Buendeln, die kleine lange Organellen genannt werden?", {"Myofilamente", "Epimysium", "Myofibrillen", "Myokard"}, 2},
  {"Was ist die atomare Kohlenstoffmasse?", {"16", "12", "14", "10"}, 1},
  {"Wie viele Planeten bilden unser Sonnensystem?", {"7", "6", "8", "9"}, 2}
};

#define NUM_Q_GESCHICHTE 44
const Question questions_Geschichte[NUM_Q_GESCHICHTE] = {
  {"Wann hat Litauen die Unabhaengigkeit der Sowjetunion erklaert?", {"11. Maerz 1990", "20. April 1989", "5. Dezember 1991", "25. Dezember 1991"}, 0},
  {"Welche dieser Gruendervaeter der Vereinigten Staaten von Amerika wurde spaeter Praesident?", {"James Monroe", "Samuel Adams", "Roger Sherman", "Alexander Hamilton"}, 0},
  {"In welchem Jahr ist Texas aus Mexiko abgelaufen?", {"1845", "1844", "1836", "1838"}, 2},
  {"Wer war der Anfuehrer der Kommunistischen Partei Jugoslawiens?", {"Josip Broz Tito", "Aleksandar Petrovic", "Milos Obilic", "Karadjordje Petrovic"}, 0},
  {"Was war William Frederick Cody besser bekannt?", {"Billy das Kind", "Wild Bill Hickok", "Buffalo Bill", "Pawnee Bill"}, 2},
  {"Wie heisst das Schiff, das nur wenige Meilen vom RMS -Titanic entfernt war, als es am 14. April 1912 einen Eisberg traf?", {"Karpaten", "Cristol", "Handel", "kalifornisch"}, 3},
  {"Welches moderne Land ist die Region, die in der Antike als Phrygie bekannt war?", {"Griechenland", "Syrien", "Aegypten", "Truthahn"}, 3},
  {"Wie heisst es richtig?", {"Tem & Uuml; Jin", "Tem & uuml; r", "& OUml; Geedei", "M & OUMl; Ngke"}, 0},
  {"Wie hiess der Spionagering, der den Vereinigten Staaten dazu beitrug, den Unabhaengigkeitskrieg zu gewinnen?", {"Unbenannt", "Washingtons Spione", "Culper Ring", "New Yorker Spionagering"}, 2},
  {"In welchem Alter war Koenig Henry V, als er starb?", {"62", "73", "35", "87"}, 2},
  {"Der Voelkermord Herero wurde in Afrika durch welche der folgenden Kolonialnationen veruebt?", {"Grossbritannien", "Belgien", "Deutschland", "Frankreich"}, 2},
  {"Das Pantheon in Rom wurde verwendet, um den Gott zu verehren?", {"Sowohl Athena als auch Zeus", "Zeus", "Athena", "Jeder Gott, den sie wollten"}, 3},
  {"Bevor die amerikanischen Kolonien 1752 in den Gregorianischen Kalender umstellten, begann ihr neues Jahr?", {"1. Dezember", "25. Maerz", "1. Juni", "25. September"}, 1},
  {"Welche dieser Einrichtungen war auf der Titanic nicht vorhanden?", {"Kuerbisgericht", "Zwinger", "Ohnmachtraum", "Tuerkisches Bad"}, 2},
  {"Wie viele bemannte Mondlandungen gab es?", {"3", "1", "6", "7"}, 2},
  {"Welches Datum war der erste Flug des P-40 Warhawk?", {"14. Oktober 1938", "21. August 1939", "1. Juni 1939", "12. Januar 1940"}, 0},
  {"An welchem Tag begann der Versuch des Versuchs, 1991 in der Sowjetunion begonnen zu haben?", {"24. Dezember", "21. August", "19. August", "26. Dezember"}, 2},
  {"Alle folgenden Namen sind Namen der sieben kriegfuehrenden Staaten ausser:", {"Qin (Qin)", "Zhao (Zhao)", "Zhai (Zhai)", "Qi (Qi)"}, 2},
  {"In welchem Jahr wurde die Unabhaengigkeitserklaerung der Vereinigten Staaten unterzeichnet?", {"1777", "1775", "1774", "1776"}, 3},
  {"Der boehmische Revolte (1618-1620) begann, nachdem die Protestanten in Prag, was fuer ihre katholischen Regentien der Lords getan hat?", {"Sperrte sie in Aktienaden", "Beleidigte ihre Muetter", "Haengte sie.", "Warf sie aus einem Fenster"}, 3},
  {"Welcher Tank Ace des Zweiten Weltkriegs wird zugeschrieben, dass er die meisten Panzer zerstoert hat?", {"Kurt Knispel", "Walter Kniep", "Otto Carius", "Michael Wittmann"}, 0},
  {"Das ontologische Argument fuer den Beweis der Existenz Gottes wird zuerst wem zugeschrieben?", {"Anselm von Canterbury", "Immanuel Kant", "Aristoteles", "Ren & eacute;Descartes"}, 0},
  {"Was hat die Abkuerzung?Im Jahr 1912 fuer die RMS -Titanic stehen?", {"Regulierter Strudelsensor", "Regelmaessiger maritimer Zeitplan", "Royal Mail Ship", "Royal Majesty Service"}, 2},
  {"Wer ist der Schoepfer des Erfrischungsgetraenks, Dr. Pepper?", {"James Wellington", "Johnson Hinsin", "Charles Alderton", "Boris Heviltik"}, 2},
  {"Was haben die ersten Automaten in den fruehen 1880er Jahren ausgegeben?", {"Limonade", "Alkohol", "Zigaretten", "Postkarten"}, 3},
  {"Wie heisst die geplante Invasion Japans gegen Ende des Zweiten Weltkriegs?", {"Operation Boarding Party", "Operation Sturz", "Operation in Flammen", "Operation Ironclad"}, 1},
  {"Gegen das Land kaempfte die niederlaendische Republik gegen die achtzig Jahre 'Krieg?", {"Spanien", "Portugal", "England", "Frankreich"}, 0},
  {"Welche amerikanische Zivilisation ist die Quelle der Ueberzeugung, dass die Welt am 21. Dezember 2012 enden oder drastisch veraendern wuerde?", {"Die Azteken", "Die Inkas", "Die Navajos", "Die Mayas"}, 3},
  {"Was war die letzte Kolonie, die in Grossbritannien das Ende des British Empire markierte?", {"Indien", "Australien", "Irland", "Hongkong"}, 3},
  {"Wer war 1871 der erste Kanzler eines vereinten Deutschlands?", {"Kaiser Wilhelm", "Otto von Bismarck", "Friedrich der 2.", "Robert Koch"}, 1},
  {"Wann wurde Google gegruendet?", {"12. Dezember 1989", "7. Februar 2000", "9. Oktober 1997", "4. September 1998"}, 3},
  {"Wann endete die Schlacht der Ausbuchtung?", {"6. August 1945", "25. Januar 1945", "7. Dezember 1941", "16. Dezember 1944"}, 1},
  {"Der betruegerische Arzt John R. Brinkley hat Anfang des 20. Jahrhunderts grossen Ruhm und Reichtum angesammelt. Welchen Service hat er angeboten?", {"Kuhlebertransplantation", "Truthahn -Brustimplantate", "Ziegenhoden transplantieren", "Schweinbluttransfusion"}, 2},
  {"Der dreissigjaehrige Krieg endete mit welchem Vertrag?", {"Frieden von Prag", "Vertrag von Versailles", "Vertrag von Paris", "Frieden der Westfalia"}, 3},
  {"Welche der folgenden Aussagen wird nicht als semitische Sprache eingestuft?", {"Akkadian", "Sumerianer", "maltesisch", "Mandaik"}, 1},
  {"An welchem Tag begann der Weltkrieg?", {"28. Januar", "28. April", "28. Juni", "28. Juli"}, 3},
  {"Welches Las Vegas Casino wurde urspruenglich von Mobster Bugsy Siegel gebaut und betrieben?", {"Die Sahara", "Der Sand", "Der MGM Grand", "Der Flamingo"}, 3},
  {"Welcher beruehmte Militaerkommandant marschierte eine Armee, zu der Kriegselefanten gehoerten, ueber die Alpen waehrend des zweiten punischen Krieges?", {"Hannibal", "Garmanicus", "Tiberius", "Alexander der Grosse"}, 0},
  {"Wer hat den \"Flying Shuttle\" erfunden?1738;Eine der wichtigsten Entwicklungen bei der Industrialisierung des Webens?", {"John Deere", "Richard Arkwright", "James Hargreaves", "John Kay"}, 3},
  {"Der Sohn, von dem Papst angeblich einen lechous f & ecirc;", {"Unschuldig v", "Urban II", "Pius III", "Alexander VI"}, 3},
  {"Welches der folgenden Laender war waehrend des Zweiten Weltkriegs keine Achsenmacht?", {"Deutschland", "Italien", "Japan", "die Sowjetunion"}, 3},
  {"Wie alt war Lyndon B. Johnson, als er die Rolle des Praesidenten der Vereinigten Staaten uebernahm?", {"54", "55", "60", "50"}, 1},
  {"In welchem Dialog hat sich Sokrates vor dem Gericht von Athen verteidigt?", {"Die Entschuldigung", "Die Euthyphro", "Die Republik", "Die Gesetze"}, 0},
  {"Wer war der erste Praesident, der in den unabhaengigen Vereinigten Staaten geboren wurde?", {"George Washington", "Martin van Buren", "James Monroe", "John Adams"}, 1}
};

#define NUM_Q_GEOGRAPHIE 47
const Question questions_Geographie[NUM_Q_GEOGRAPHIE] = {
  {"Welches Land war nicht Teil der Sowjetunion?", {"Rumaenien", "Turkmenistan", "Weissrussland", "Tadschikistan"}, 0},
  {"In welchem Kontinent befindet sich das Land Lesotho?", {"Afrika", "Europa", "Suedamerika", "Asien"}, 0},
  {"Wie heisst die kanadische Nationalhymne?", {"Marsch des Puck Drops", "O roter Ahorn", "Blattspangte Banner", "O Kanada"}, 3},
  {"Welche Farbe hat der Kreis auf der japanischen Flagge?", {"Gelb", "Weiss", "Rot", "Schwarz"}, 2},
  {"Welche dieser amerikanischen Staedte hat weniger als 1.000.000 Menschen?", {"San Francisco, Kalifornien", "Philadelphia, Pennsylvania", "Phoenix, Arizona", "San Antonio, Texas"}, 0},
  {"Welches Land ist nicht Teil von Skandinavien?", {"Finnland", "Daenemark", "Norwegen", "Schweden"}, 0},
  {"Bridgetown ist die Hauptstadt welcher Inselland in der Karibik?", {"Jamaika & LRM;", "Barbados", "Kuba", "Dominica"}, 1},
  {"Was ist die Hauptstadt Suedkoreas?", {"Pjoengjang", "Kitakyushu", "Seoul", "Taegu"}, 2},
  {"Was ist die Hauptstadt des amerikanischen Bundesstaates Arizona?", {"Raleigh", "Tallahassee", "Montgomery", "Phoenix"}, 3},
  {"In welchem Land befindet sich Tallinn?", {"Estland", "Polen", "Schweden", "Finnland"}, 0},
  {"Was ist die Hauptstadt Australiens?", {"Melbourne", "Brisbane", "Canberra", "Sydney"}, 2},
  {"Was ist der groesste Suesswassersee nach Volumen?", {"Huronsee", "Lake Superior", "Lake Baikal", "Lake Michigan"}, 2},
  {"Was ist der einzige Staat in den Vereinigten Staaten, der keine Flagge in einer Form mit 4 Kanten hat?", {"Florida", "Idaho", "Ohio", "New Mexico"}, 2},
  {"Welche kanadische Provinz hat Charlottetown als Hauptstadt?", {"Nordwestterritorien", "Saskatchewan", "Prinz Edward Island", "Ontario"}, 2},
  {"Wie heisst die Rocky -Region, die den groessten Teil Ostkanadas erstreckt?", {"Appalachische Berge", "Rocky Mountains", "Himalaya", "Kanadischer Schild"}, 3},
  {"Welche davon ist keine Insel, die Teil der Philippinen ist?", {"Luzon", "Palawan", "Java", "Mindanao"}, 2},
  {"Wie viele unabhaengige Laender gibt es auf dem Kontinent Suedamerikas?", {"10", "12", "8", "9"}, 1},
  {"Was ist das fuenftgroesste Land von Gebiet?", {"Vereinigte Staaten", "Brasilien", "Indien", "Australien"}, 1},
  {"Alle folgenden Laender haben offizielle Ansprueche auf Territorium in Antartica, ausser:", {"Norwegen", "Vereinigte Staaten", "Chile", "Australien"}, 1},
  {"2012 die deutschsprachige Mikrostata & quot; Liechtenstein & quot;In Mitteleuropa hatte eine Bevoelkerung von wie vielen Einwohnern?", {"36.600", "2.400", "90.000", "323.400"}, 0},
  {"Welches Land hat die Abkuerzung \"ch & quot;?", {"Schweiz", "Kein Land", "China", "Kanada"}, 0},
  {"Wie viele Provinzen sind in den Niederlanden?", {"10", "13", "14", "12"}, 3},
  {"Welche dieser Insellaender befindet sich in der Karibik?", {"Barbados", "Fidschi", "Malediven", "Seychellen"}, 0},
  {"Die Hunua -Bereiche befinden sich in ...", {"China", "Nepal", "Mexiko", "Neuseeland"}, 3},
  {"Welches Land im Vereinigten Koenigreich erscheint nicht auf seiner Flagge, der Union Jack?", {"Isle of Wight", "Schottland", "Wales", "Irland"}, 2},
  {"Was ist das Land, das Nordamerika und Suedamerika verbindet?", {"Isthmus von Panama", "Isthmus von Suez", "Australasien", "Ural"}, 0},
  {"Welches afrikanische Land hat Portugiesisch als offizielle Sprache?", {"Gabon", "Botswana", "Mosambik", "Togo"}, 2},
  {"Was ist der richtige Weg, um die Hauptstadt Ungarns zu buchstabieren?", {"Budapest", "Budapest", "Budapest", "Budapest"}, 0},
  {"Wie hoch waren in der Global Peace Index -Umfrage 2016 von 163 Laendern die Vereinigten Staaten von Amerika eingestuft?", {"103", "79", "10", "59"}, 0},
  {"Wo befindet sich Hadrians Wand?", {"Carlisle, England", "Dublin, Irland", "Alexandria, Aegypten", "Rom, Italien"}, 0},
  {"Wo befindet sich die aelteste noch operative Raumstarteinrichtung der Welt?", {"Russland", "Iran", "Kasachstan", "Vereinigte Staaten"}, 2},
  {"Llanfair & shy;", {"Caldey", "Bardsey", "Barry", "Anglesey"}, 3},
  {"Welches europaeische Land ist nicht Teil der EU?", {"Tschechien", "Litauen", "Norwegen", "Irland"}, 2},
  {"Welche davon ist keine Provinz in China?", {"Sichuan", "Fujian", "Yangtze", "Guangdong"}, 2},
  {"Was ist das finnische Wort fuer \"Finnland\"?", {"Magyarorsz & aacute; g", "Sverige", "Suomi", "Eesti"}, 2},
  {"Mit wie vielen Laendern teilen die Vereinigten Staaten eine Landgrenze?", {"3", "1", "4", "2"}, 3},
  {"Was ist das groesste Land der Welt?", {"Brasilien", "Kanada", "Russische Foederation", "China"}, 2},
  {"Wie viele Zeitzonen gibt es in Russland?", {"5", "11", "8", "2"}, 1},
  {"Das Land von Gotland befindet sich in welchem europaeischen Land?", {"Norwegen", "Daenemark", "Schweden", "Deutschland"}, 2},
  {"Wie viele Zeitzonen hat China?", {"3", "4", "2", "1"}, 3},
  {"Gibraltar, suedlich der iberischen Halbinsel, ist ein Gebiet von welchem Westen Europas?", {"Portugal", "Vereinigtes Koenigreich", "Spanien", "Frankreich"}, 1},
  {"Welche Stadt hat den am staerksten verkehrsreichsten Flughafen der Welt?", {"London, England", "Atlanta, Georgia USA", "Chicago, Illinois Isa", "Tokio, Japan"}, 1},
  {"Groenland ist fast so gross wie Afrika", {"?", "False", "True", "?"}, 1},
  {"Welche der folgenden europaeischen Sprachen wird als \"Sprachisolat\" eingestuft?", {"galizisch", "ungarisch", "baskisch", "maltesisch"}, 2},
  {"Was ist Laos?", {"Land", "Stadt", "Region", "Fluss"}, 0},
  {"Was ist die Hauptstadt British Columbia, Kanada?", {"Kelowna", "Victoria", "Vancouver", "Hoffnung"}, 1},
  {"Welcher der folgenden japanischen Inseln ist die groessten?", {"Honshu", "Shikoku", "Kyushu", "Hokkaido"}, 0}
};

#define NUM_Q_FAHRZEUGE 11
const Question questions_Fahrzeuge[NUM_Q_FAHRZEUGE] = {
  {"Der LS7 -Motor ist wie viele Kubikzoll?", {"364", "427", "376", "346"}, 1},
  {"Welche Art von Zug war Stepney, ein Zug auf der Bluebell Railway, der fuer seinen Auftritt in \"The Railway Series\" bemerkt wurde?", {"Lb & amp; scr j1", "Lb & amp; scr e2", "Lb & amp; scr a1x", "Lb & amp; scr d1"}, 2},
  {"Wann wurde Tesla gegruendet?", {"2005", "2007", "2008", "2003"}, 3},
  {"In welchem der folgenden Fahrzeuge verfuegte am Basismodell ein volles Glasdach?", {"Renault Avantime", "Mercedes-Benz A-Klasse", "Chevy Volt", "Honda Odyssey"}, 0},
  {"Welches Tiermerkmal auf dem Logo fuer Abarth, die Motorsport -Division von Fiat?", {"Stier", "Schlange", "Skorpion", "Pferd"}, 2},
  {"Welches japanische Unternehmen ist der weltweit groesste Hersteller von Motorraedern?", {"Yamaha", "Honda", "Suzuki", "Kawasaki"}, 1},
  {"Jaguar Cars war zuvor im Besitz von welchem Autohersteller?", {"Chrysler", "Ford", "General Motors", "Fiat"}, 1},
  {"Welches ist nicht die Funktion von Motoroel in Automotoren?", {"Verbrennung", "Kuehlung", "Korrosion reduzieren", "Schmierung"}, 0},
  {"Welches der folgenden Laender hat den zivilen Einsatz von Dash -Cams in Autos offiziell verboten?", {"Tschechien", "Oesterreich", "Vereinigte Staaten", "Suedkorea"}, 1},
  {"Welcher Motor ist im Lexus SC400?", {"1UZ-FE", "7m-gte", "5m-GE", "2jz-gte"}, 0},
  {"In welchem Land ist der Hussarya Supercar, der vom Automobilhersteller hergestellt wurde.", {"Schweden", "Polen", "China", "Italien"}, 1}
};

#define NUM_Q_SPORT 27
const Question questions_Sport[NUM_Q_SPORT] = {
  {"In welchem Jahr fand der dritte Super Bowl statt?", {"1971", "1969", "1970", "1968"}, 1},
  {"Wer hat die National Championship 2015 College Football Playoff (CFP) gewonnen?", {"Wisconsin Badgers", "Clemson Tiger", "Ohio State Buckeyes", "Alabama Crimson Tide"}, 2},
  {"In welchem Jahr haben die New Orleans Saints den Super Bowl gewonnen?", {"2009", "2010", "2011", "2008"}, 1},
  {"Wie oft muss der Ball im Baseball ausserhalb der Streikzone aufgestellt werden, bevor der Teig gegangen ist?", {"3", "2", "4", "1"}, 2},
  {"Welche dieser russischen Staedte enthielt kein Stadion, das bei der FIFA -Weltmeisterschaft 2018 verwendet wurde?", {"Vladiwostok", "Rostov-on-don", "Kaliningrad", "Yekaterinburg"}, 0},
  {"Welcher italienische Fussballer sagte zu Neuer, wo er waehrend des Spiels Italy-Germany, UEFA Euro 2016, seinen Schuss und die Breite des Spiels breit hat?", {"Barzagli", "Giaccherini", "Insigne", "Pelle"}, 3},
  {"Wie wird im Bowling der Begriff verwendet, um drei aufeinanderfolgende Streiks zu erhalten?", {"Truthahn", "Birdie", "Adler", "Flamingo"}, 0},
  {"Welche der folgenden Grand Slam -Tennis -Turniere finden zuletzt statt?", {"Franzoesisch offen", "US Open", "Wimbledon", "Australian Open"}, 1},
  {"Wer hat 2017 die UEFA Champions League gewonnen?", {"Juventus F.C.", "Als Monaco FC", "Real Madrid C.F.", "Atletico Madrid"}, 2},
  {"Welches Logo des deutschen Sportswear -Unternehmens ist das Formenstreifen '?", {"Nike", "Adidas", "Puma", "Reebok"}, 2},
  {"In der Formel 1 wurde das virtuelle Sicherheitsauto nach dem toedlichen Absturz von welchem Fahrer eingefuehrt?", {"Ayrton Senna", "Gilles Villeneuve", "Ronald Renzenberger", "Jules Bianchi"}, 3},
  {"Welches Tool verleiht es zu einem Vorsprung des Last-Stone-Vorteils am Ende des Curlings?", {"Schluessel", "Hammer", "Schraubendreher", "Bohren"}, 1},
  {"Welche Nation hat 2006 die FIFA -Weltmeisterschaft veranstaltet?", {"Vereinigtes Koenigreich", "Brasilien", "Suedafrika", "Deutschland"}, 3},
  {"Welches Team war der NBA-Meister 2014-2015?", {"Houston Rockets", "Golden State Warriors", "Atlanta Hawks", "Cleveland Cavaliers"}, 1},
  {"Auf welcher Bruecke beginnt das jaehrliche Bootsrennen von Oxford und Cambridge?", {"Hammersmith", "Battersea", "Vauxhall", "Putney"}, 3},
  {"Wie viele Fussballspieler sollten gleichzeitig auf dem Feld sein?", {"22", "20", "24", "26"}, 0},
  {"Welches Fussballteam gewann die Copa Am & Eacute; Rica Centenario 2016?", {"Chile", "Argentinien", "Kolumbien", "Brasilien"}, 0},
  {"Welches Team hat die englische Premier League 2015-16 gewonnen?", {"Leicester City", "Manchester United", "Liverpool", "Chelsea"}, 0},
  {"Welches Team hat die meisten Stanley Cups in der NHL gewonnen?", {"Detroit Red Wings", "Toronto Maple Leafs", "Montreal Canadiens", "Chicago Blackhawks"}, 2},
  {"Aus welchem Land stammte Kabaddi, ein Kontaktsport, aus dem sich angeht, aus?", {"Australien", "Truthahn", "Indien", "Kambodscha"}, 2},
  {"Welches Team gewann England im Weltcup -Finale von 1966?", {"Portugal", "die Sowjetunion", "Brasilien", "Westdeutschland"}, 3},
  {"In welchem Jahr wurde die Hockeylegende Wayne Gretzky geboren?", {"1963", "1961", "1959", "1965"}, 1},
  {"Warum wurde das gruene Monster im Fenway Park urspruenglich gebaut?", {"Um zu verhindern, dass Spiele von ausserhalb des Parks betrachtet werden.", "Anzeigen von Anzeigen.", "Homeraufen zu machen.", "Zusaetzliche Sitzplaetze bereitstellen."}, 0},
  {"Wie viele Punktzonen gibt es auf einem konventionellen Dart -Board?", {"82", "62", "42", "102"}, 0},
  {"Wer ist Manchester Uniteds fuehrender Erscheinungsmacher?", {"David Beckham", "Ryan Giggs", "Eric Cantona", "Wayne Rooney"}, 1},
  {"Was ist der Sport der Koenige & rdquo;?", {"Schach", "Pferderennen", "Turnier", "Fechten"}, 1},
  {"Was ist das aelteste Team in der NFL?", {"Chicago Bears", "Green Bay Packers", "Arizona Cardinals", "New York Giants"}, 2}
};

#define NUM_Q_PROMINENTE 13
const Question questions_Prominente[NUM_Q_PROMINENTE] = {
  {"Was wird allgemein als Geburtsdatum von William Shakespeare angesehen?", {"4. Juli 1409", "23. April 1564", "1. Dezember 1750", "29. September 1699"}, 1},
  {"Was war der letzte Filmrolle von James Coburn vor seinem Tod?", {"Monsters Inc", "Schneehunde", "Texas Rangers", "Amerikanische Waffe"}, 3},
  {"Paul McCartney hat immer seinen zweiten Vornamen verwendet.Was ist sein wahrer Vorname?", {"John", "Justin", "Jack", "James"}, 3},
  {"Welche amerikanische Prominente starb 1977 in La Moraleja, Madrid, Golf?", {"Charlie Chaplin", "Bing Crosby", "Groucho Marx", "Elvis Presley"}, 1},
  {"Wie alt war Muhammad Ali, als er starb?", {"56", "74", "Er lebt noch", "61"}, 1},
  {"Wer war die erste Frau von Donald Trump?", {"Nancy Davis", "Ivana Zeln & iacute; ckov & aacute;", "Melania Knauss", "Marla Maples"}, 1},
  {"Wann wurde Elvis Presley geboren?", {"18. Juli 1940", "8. Januar 1935", "13. Dezember 1931", "17. April 1938"}, 1},
  {"Gabe Newell wurde in welchem Jahr geboren?", {"1962", "1960", "1972", "1970"}, 0},
  {"Der exzentrische Naturphilosoph Tycho Brahe hielt was als Haustier?", {"Ziege", "Tragen", "Hund", "Elch"}, 3},
  {"In welchem Jahr hat Radio Icon Howard Stern einen Job beim Radiosender WNBC begonnen?", {"1995", "1982", "1985", "1984"}, 1},
  {"Neil Hamburger wird von welchem Komiker gespielt?", {"Gregg Turkington", "Nathan Fielder", "Tim Heidecker", "Toddglas"}, 0},
  {"Worauf konzentriert sich der Filmemacher Dan Bell normalerweise auf seine Filme?", {"Historische Sehenswuerdigkeiten", "Actionfilme", "Dokumentarfilme", "Verlassene Gebaeude und tote Einkaufszentren"}, 3},
  {"Wenn er noch am Leben waere, wuerde Elvis Presley in welchem Jahr seinen 100. Geburtstag feiern?", {"2045", "2030", "2040", "2035"}, 3}
};

#define NUM_Q_TIERE 12
const Question questions_Tiere[NUM_Q_TIERE] = {
  {"Welche davon ist eine Kolonie von Polypen und keine Quallen?", {"Sea Wesp", "Irukandji", "Meeresbrennnessel", "Portugiesisch-Kriegsmann"}, 3},
  {"Was ist die laengste giftige Schlange der Welt?", {"Koenig Cobra", "Gruene Anaconda", "Gelbe Bellige Meerschlange", "Inland Taipan"}, 0},
  {"Was ist das kollektive Substantiv fuer eine Gruppe von Kraehen?", {"Herde", "Schar", "Mord", "Pack"}, 2},
  {"Welcher dieser Tiere ist keine Eidechse?", {"Gila Monster", "Komodo Drache", "Gruene Leguan", "Tuatara"}, 3},
  {"Wie hiess der aethiopische Wolf, bevor sie wussten, dass er mit Woelfen zusammenhaengt?", {"Canis simiensis", "Amharischer Fuchs", "Simien Jackel", "Aethiopischer Kojote"}, 2},
  {"Wie heisst die Familie, dass die Hauskatze Mitglied ist?", {"Felidae", "Felinae", "Katze", "Felis"}, 0},
  {"Wie heisst das kupferreiche Protein, das das blaue Blut im antarktischen Oktopus erzeugt?", {"Cytochrom", "Haemocyanin", "Methionin", "Eisen"}, 1},
  {"Hippocampus ist der lateinische Name fuer welche marine Kreatur?", {"Oktopus", "Wal", "Seepferdchen", "Delphin"}, 2},
  {"Was ist das kollektive Substantiv fuer Baeren?", {"Fuhr", "Schale", "Faultier", "Stamm"}, 2},
  {"Wie heisst der wissenschaftliche Name des Wellenlochs?", {"Ara Macao", "Pyrrhura Molinae", "Nymphicus hollandicus", "Melopsittacus undulatus"}, 3},
  {"Ein fleischfressendes Tier frisst Fleisch, was frisst ein nukangeloeses Tier?", {"Nuesse", "Obst", "Algen", "Nichts"}, 0},
  {"Welches Tier war 1959 Teil eines russischen Domestizierungsexperiments?", {"Alligatoren", "Baeren", "Tauben", "Fuechse"}, 3}
};

#define NUM_Q_ALLGEMEINWISSEN 47
const Question questions_Allgemeinwissen[NUM_Q_ALLGEMEINWISSEN] = {
  {"Welcher davon ist der Name eines japanischen Systems alternativer Medizin, das buchstaeblich \"Fingerdruck\" bedeutet \"?", {"Ukiyo", "Majime", "Ikigai", "Shiatsu"}, 3},
  {"Die Fields -Medaille, eine der gefragtesten Auszeichnungen in der Mathematik, wird alle Jahre verliehen?", {"3", "6", "5", "4"}, 3},
  {"Terry Gilliam war ein Animator, der mit welcher britischen Comedy -Gruppe arbeitete?", {"Monty Python", "Die Goodies & LRM;", "Die Liga der Herren & LRM;", "Die Penny Dreadfuls"}, 0},
  {"Welcher der folgenden ist eine vorhandene Familie in \"The Sims\"?", {"Die Goth -Familie", "Die Familie", "Die Familie Simoleon", "Die stolze Familie"}, 0},
  {"Wo hat Apple Inc. seinen Hauptsitz?", {"Redmond, Washington", "Cupertino, Kalifornien", "Redwood City, Kalifornien", "Santa Monica, ca."}, 1},
  {"Was ist das Gewicht einer Goldbar in Fallout: New Vegas?", {"30 Pfund", "35 Pfund", "32,50 Pfund", "40 Pfund"}, 1},
  {"Was war das Ziel des fehlenden Fluges MH370?", {"Tokio", "Singapur", "Kuala Lumpur", "Peking"}, 3},
  {"Welcher italienische Automobilhersteller hat 2011 die Mehrheitsbetrieb des US -amerikanischen Automobilherstellers Chrysler erlangt?", {"Fiat", "Ferrari", "Maserati", "Alfa Romeo"}, 0},
  {"Wie viele Farben gibt es in einem Regenbogen?", {"9", "10", "7", "8"}, 2},
  {"Wie ist die Form des Spielzeugs, das ungarischer Professor Erno Rubik erfunden wurde?", {"Wuerfel", "Pyramide", "Zylinder", "Kugel"}, 0},
  {"Apple-Mitbegruender Steve Jobs starb an Komplikationen fuer die Krebsform?", {"Leber", "Knochen", "Magen", "Pankreas"}, 3},
  {"Wann wurde die PlayStation 3 veroeffentlicht?", {"25. Dezember 2007", "8. Januar 2007", "11. November 2006", "16. Juli 2006"}, 2},
  {"In welcher Richtung steht die Freiheitsstatue aus?", {"Nordwest", "Suedost", "Nordost", "Suedwesten"}, 1},
  {"Welche Fast -Food -Kette hat weltweit die meisten Orte?", {"U -Bahn", "Starbucks", "KFC", "MC Donalds"}, 0},
  {"In welchem Jahr wurde der Robosapien -Spielzeugroboter veroeffentlicht?", {"2001", "2006", "2004", "2000"}, 2},
  {"Wie lange muessen Sie laut Fair Works Australia arbeiten, um einen langen Service zu erhalten?", {"2 Jahre", "6 Monate", "8 Jahre", "7 Jahre"}, 3},
  {"Wer hat 1847 die erste Schokoladenbar erfunden?", {"John Tyler", "Andrew Johnson", "Joseph Fry", "John Cadbury"}, 2},
  {"Welche dieser Banken sind nicht berechtigt, Waehrungsnotizen in Hongkong auszustellen?", {"OCBC", "Bank of China", "HSBC", "Standard gechartert"}, 0},
  {"Welches Land, nicht in Japan, hat die meisten Menschen japanischer Abstammung?", {"Vereinigte Staaten von Amerika", "Brasilien", "Suedkorea", "China"}, 1},
  {"Wie lautete der urspruengliche Name der Suchmaschine \"Google\"?", {"Netscape Navigator", "Catmassage", "SearchPro", "BackRub"}, 3},
  {"Woher stammt die Ananaspflanze?", {"Asien", "Suedamerika", "Europa", "Hawaii"}, 1},
  {"In welchem Land wurde am 20. Mai 1991 ein Weltrekord von 315 Millionen Waehlern fuer Wahlen ausgewiesen?", {"Polen", "Vereinigte Staaten von Amerika", "die Sowjetunion", "Indien"}, 3},
  {"Wenn jemand feige ist, wird gesagt, dass er welchen Farbbauch hat?", {"Blau", "Rot", "Gelb", "Gruen"}, 2},
  {"Was ist \"tupfen\"?", {"Ein Sport", "Eine Sprache", "Ein medizinisches Verfahren", "Ein Tanz"}, 3},
  {"Welche der folgenden Sprachen verwendet das Geschlecht nicht als Teil seiner Grammatik?", {"Deutsch", "Polieren", "daenisch", "Tuerkisch"}, 3},
  {"Welches Wort repraesentiert den Buchstaben ' t 'im NATO -phonetischen Alphabet?", {"Taxi", "Truthahn", "Tango", "Ziel"}, 2},
  {"In dem Video-Game-Franchise Kingdom Hearts traegt der Hauptdarsteller eine Waffe mit welcher Form?", {"Handy", "Stift", "Schwert", "Schluessel"}, 3},
  {"Welche dieser Farben wird im Logo fuer Google nicht vorgestellt?", {"Rosa", "Blau", "Gruen", "Gelb"}, 0},
  {"Welcher amerikanische Herstellungs -Untermaachine -Waffe wurde von den amerikanischen Soldaten informell bekannt, die sie als \"Grease Gun & Quot;?", {"Colt 9mm", "Thompson", "M3", "MAC-10"}, 2},
  {"Was ist das romanisierte chinesisches Wort fuer \"Flugzeug\"?", {"Feiji", "Huojian", "Zongxian", "Qiche"}, 0},
  {"Was ist das spanische Wort fuer \"Esel\"?", {"Burro", "Caballo", "Toro", "Perro"}, 0},
  {"Was war der Spitzname, der den Hughes H-4 Hercules, einem schweren Transportboot, das 1947 Flug erreichte, gegeben wurde?", {"Noahs Arche", "Fichte Gans", "Dicker Mann", "Trojanisches Pferd"}, 1},
  {"Der Computerhersteller Compaq wurde 2002 fuer 25 Milliarden US -Dollar von welchem Unternehmen uebernommen?", {"Dell", "Hewlett-Packard", "Asus", "Toshiba"}, 1},
  {"Welches Unternehmen hat das Vocaloid Hatsune Miku entwickelt?", {"Crypton Future Media", "Yamaha Corporation", "Sega", "Sony"}, 0},
  {"Sciophobie ist die Angst vor was?", {"Schatten", "Transport", "Essen", "Helle Lichter"}, 0},
  {"Wie hiess das WWF Professional Wrestling Tag -Team, das aus den Wrestlern Axt und Smash bestand?", {"Die britischen Bulldoggen", "Das Dreamteam", "Die Bushwhacker", "Abriss"}, 3},
  {"In \"Battle Cats\", was ist der kritische prozentuale Rate von Moneko / Miss Moneko?", {"25%", "15%", "20%", "10%"}, 1},
  {"Virtual Reality Company Oculus VR verlor die Mitbegruender von IT 039 bei einem Freak-Autounfall im Jahr 2013?", {"Palmer Luckey", "Jack McCauley", "Andrew Scott Reis", "Nate Mitchell"}, 2},
  {"Was geht ein Funambulist?", {"Glasscherben", "Der Mond", "Baelle", "Ein enges Seil"}, 3},
  {"Wer ist auf der US -amerikanischen US -Dollar -Rechnung abgebildet?", {"Thomas Jefferson", "George Washington", "Abraham Lincoln", "Benjamin Franklin"}, 3},
  {"Welcher Name repraesentiert den Brief \"m & quot;im NATO -phonetischen Alphabet?", {"Matthew", "Markieren", "Mikrofon", "Max"}, 2},
  {"Welches Zeichen des Tierkreises kommt zwischen Jungfrau und Skorpion?", {"Steinbock", "Zwillinge", "Stier", "Waage"}, 3},
  {"Wie heisst das erste Video auf YouTube hochgeladen?", {"Ich im Zoo", "Ihr neuer Welpe von Urenkel Vern.", "Carrie faehrt mit einem Lastwagen", "Tribut"}, 0},
  {"Was ist das franzoesische Wort fuer \"Hut\"?", {"& Eacute; Charpe", "Motorhaube", "Casque", "Chapeau"}, 3},
  {"Welcher Filmstar hat seine Statue am Leicester Square?", {"Charlie Chaplin", "Paul Newman", "Alfred Hitchcock", "Rowan Atkinson"}, 0},
  {"Was macht die \"g & quot;Mean in \"g-Man\"?", {"Geist", "Regierung", "Gehen", "Geronimo"}, 1},
  {"Welche Suessigkeiten wird vom Mars nicht gemacht?", {"Snickers", "Mandelfreude", "M & amp; m ' s", "Twix"}, 1}
};

#define NUM_Q_MYTHOLOGIE 8
const Question questions_Mythologie[NUM_Q_MYTHOLOGIE] = {
  {"Wer hat in der griechischen Mythologie Achilles getoetet?", {"Helen", "Paris", "Tyrannisieren", "Perikles"}, 1},
  {"Welcher nordische Gott hat ein Pferd namens Slepnir?", {"Thor", "Balder", "Frigg", "Odin"}, 3},
  {"Mit welchem Tier schlief Koenigin Pasiphae, bevor sie den Minotaurus in der griechischen Mythologie zur Welt brachte?", {"Stier", "Pferd", "Schwein", "Ochse"}, 0},
  {"Was war die Strafe fuer die Kleebigkeit von Sysiphus?", {"An einen Felsbrocken fuer die Ewigkeit gebunden und von Voegeln gepickt.", "Verflucht, um einen Felsbrocken auf einen Huegel fuer die Ewigkeit zu rollen.", "Ein Baum fiel, der sich nach jeder Axtschwung regenerierte.", "In einem mit Wasser gefuellten See konnte er nicht trinken."}, 1},
  {"Wer ist der Gott des Krieges in der polynesischen Mythologie?", {"' Oro", "Maui", "Kohara", "Hina"}, 0},
  {"Der griechische Gott Poseidon war der Gott von was?", {"Sonne", "Feuer", "Krieg", "Das Meer"}, 3},
  {"Talos, der mythische Riesenbronze -Mann, war der Beschuetzer von welcher Insel?", {"Sardinien", "Kreta", "Sizilien", "Zypern"}, 1},
  {"Wer hat nach dem aegyptischen Mythos von Osiris Osiris ermordet?", {"Horus", "Anhur", "Satz", "Ra"}, 2}
};

#define NUM_Q_POLITIK 7
const Question questions_Politik[NUM_Q_POLITIK] = {
  {"Welcher der folgenden argentinischen Praesidenten wurde 2015 gewaehlt?", {"Nestor Kirchner", "Mauricio Macri", "Juan Domingo Peron", "Cristina Fernandez de Kirchner"}, 1},
  {"Der Watergate -Skandal ereignete sich in welchem Jahr?", {"1973", "1972", "1971", "1974"}, 1},
  {"Wie viele Menschen sind im US -Repraesentantenhaus?", {"415", "260", "50", "435"}, 3},
  {"Wer war der 40. Praesident der USA?", {"Ronald Reagan", "Jimmy Carter", "Richard Nixon", "Bill Clinton"}, 0},
  {"Welches Land wurde zwischen 1973 und 1990 vom Diktator Augusto Pinochet regiert?", {"Nicaragua", "Aethiopien", "Indonesien", "Chile"}, 3},
  {"Wie heisst Niccol & Ograve;Die Arbeit von Machiavelli, die argumentierte, dass effektive Fuehrungskraefte ihre Gegner um jeden Preis zerstoeren mussten?", {"Der Prinz", "LOVE &#039 ;'s Labors verloren", "Wille zur Macht", "Die Vergewaltigung von Lucrece"}, 0},
  {"Im Juni 2017 brachen Saudi -Arabien und Aegypten Krawatten mit welchem Land ueber seine angebliche Unterstuetzung fuer den Terrorismus ab?", {"Bahrain", "Katar", "Vereinigte Staaten von Amerika", "Russland"}, 1}
};

// Pointer-Tabelle fuer Kategorien
const Question* CAT_QUESTIONS[NUM_CATEGORIES] = {
  nullptr,  // CAT_ALL - wird dynamisch befuellt
  questions_Unterhaltung,
  questions_Wissenschaft,
  questions_WissenschaftNatur,
  questions_Geschichte,
  questions_Geographie,
  questions_Fahrzeuge,
  questions_Sport,
  questions_Prominente,
  questions_Tiere,
  questions_Allgemeinwissen,
  questions_Mythologie,
  questions_Politik,
};

const int CAT_SIZES[NUM_CATEGORIES] = {
  0,  // CAT_ALL
  50,  // Unterhaltung
  44,  // Wissenschaft
  44,  // WissenschaftNatur
  44,  // Geschichte
  47,  // Geographie
  11,  // Fahrzeuge
  27,  // Sport
  13,  // Prominente
  12,  // Tiere
  47,  // Allgemeinwissen
  8,  // Mythologie
  7,  // Politik
};

// ── Kategorie-Auswahl ─────────────────────────────────────────────────────────
int  selectedCategory = 0;  // 0=Alle, 1..12=spezifisch
int  selectedQ[NUM_QUESTIONS_PER_GAME];

// Alle Fragen als flaches Array fuer CAT_ALL
// Wird dynamisch beim Start befuellt
#define MAX_ALL_Q 400
const Question* allPool[MAX_ALL_Q];
int allPoolSize = 0;

void buildAllPool() {
  allPoolSize = 0;
  for (int c = 1; c < NUM_CATEGORIES && allPoolSize < MAX_ALL_Q; c++) {
    for (int i = 0; i < CAT_SIZES[c] && allPoolSize < MAX_ALL_Q; i++) {
      allPool[allPoolSize++] = &CAT_QUESTIONS[c][i];
    }
  }
}

void selectQuestions() {
  randomSeed(esp_random() ^ millis());

  const Question* pool;
  int poolSize;

  if (selectedCategory == 0) {
    // Alle Kategorien gemischt
    // Shuffle allPool, dann erste 10 nehmen
    static int poolIdx[MAX_ALL_Q];
    for (int i = 0; i < allPoolSize; i++) poolIdx[i] = i;
    for (int i = allPoolSize - 1; i > 0; i--) {
      int j = random(i + 1);
      int tmp = poolIdx[i]; poolIdx[i] = poolIdx[j]; poolIdx[j] = tmp;
    }
    for (int i = 0; i < NUM_QUESTIONS_PER_GAME && i < allPoolSize; i++)
      selectedQ[i] = poolIdx[i];
    return;
  }

  // Spezifische Kategorie
  pool     = CAT_QUESTIONS[selectedCategory];
  poolSize = CAT_SIZES[selectedCategory];

  // Fisher-Yates shuffle
  static int indices[500];
  int n = min(poolSize, 499);
  for (int i = 0; i < n; i++) indices[i] = i;
  for (int i = n - 1; i > 0; i--) {
    int j = random(i + 1);
    int tmp = indices[i]; indices[i] = indices[j]; indices[j] = tmp;
  }
  for (int i = 0; i < NUM_QUESTIONS_PER_GAME && i < n; i++)
    selectedQ[i] = indices[i];
}

const Question& curQuestion(int qIdx) {
  if (selectedCategory == 0) {
    return *allPool[selectedQ[qIdx]];
  }
  return CAT_QUESTIONS[selectedCategory][selectedQ[qIdx]];
}

int questionsAvailable() {
  if (selectedCategory == 0) return allPoolSize;
  return CAT_SIZES[selectedCategory];
}

// ── Spieler ──────────────────────────────────────────────────────────────────
#define MAX_PLAYERS  8
#define MAX_NAME_LEN 24

struct Player {
  char     name[MAX_NAME_LEN];
  uint32_t ip;
  bool     active;
  int      score;
  int      answerThisRound; // -1=noch nicht, 0-3=Antwort
};

Player players[MAX_PLAYERS];
int    playerCount = 0;

int findPlayer(uint32_t ip) {
  for (int i = 0; i < playerCount; i++)
    if (players[i].ip == ip && players[i].active) return i;
  return -1;
}

int addPlayer(uint32_t ip, const char* name) {
  if (playerCount >= MAX_PLAYERS) return -1;
  int i = playerCount++;
  players[i].ip    = ip;
  players[i].active = true;
  players[i].score  = 0;
  players[i].answerThisRound = -1;
  strncpy(players[i].name, name, MAX_NAME_LEN - 1);
  players[i].name[MAX_NAME_LEN - 1] = 0;
  return i;
}

bool allAnswered() {
  if (playerCount == 0) return false;
  for (int i = 0; i < playerCount; i++)
    if (players[i].active && players[i].answerThisRound == -1) return false;
  return true;
}

void resetAnswers() {
  for (int i = 0; i < playerCount; i++)
    players[i].answerThisRound = -1;
}

// ── Spielzustand ─────────────────────────────────────────────────────────────
// 0=Lobby  1=Frage laeuft  2=Aufloesung  3=Ergebnis
int  gameState    = 0;
int  currentQ     = 0;  // Index in selectedQ[]
unsigned long questionStart    = 0;
unsigned long resolutionStart  = 0;
#define QUESTION_TIME   20000
#define RESOLUTION_TIME 10000
bool wasTouched = false;


// ── Mehrzeiliger Text ─────────────────────────────────────────────────────────
// Gibt neue Y-Position zurueck
int drawWrapped(const char* txt, int x, int y, int maxW, int lineH) {
  String s = String(txt);
  while (s.length() > 0) {
    int cut = s.length();
    while (cut > 0 && tft.textWidth(s.substring(0, cut)) > maxW) cut--;
    if (cut < (int)s.length()) {
      int sp = s.lastIndexOf(' ', cut);
      if (sp > 0) cut = sp;
    }
    tft.setTextDatum(TL_DATUM);
    tft.drawString(s.substring(0, cut), x, y);
    s = s.substring(cut); s.trim();
    y += lineH;
    if (y > SCREEN_H - 10) break;
  }
  return y;
}

// ── Display-Screens ──────────────────────────────────────────────────────────
// Kategorie-Pfeil-Buttons zeichnen (nur Kategorie-Bereich)
void drawCategoryBar() {
  int barY = SCREEN_H - 56;
  int barH = 28;
  tft.fillRect(0, barY, SCREEN_W, barH, COL_BG);

  // Linker Pfeil
  tft.fillRoundRect(4, barY + 2, 28, 24, 4, COL_CARD);
  tft.setTextColor(COL_TEXT, COL_CARD);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("<", 18, barY + 14);

  // Rechter Pfeil
  tft.fillRoundRect(SCREEN_W - 32, barY + 2, 28, 24, 4, COL_CARD);
  tft.setTextColor(COL_TEXT, COL_CARD);
  tft.drawString(">", SCREEN_W - 18, barY + 14);

  // Kategorie-Name in der Mitte
  tft.fillRect(36, barY, SCREEN_W - 72, barH, COL_BG);
  tft.setTextColor(COL_YELL, COL_BG);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(CATEGORY_NAMES[selectedCategory], SCREEN_W / 2, barY + 14);

  // Punkte-Indikator
  int totalDots = min(NUM_CATEGORIES, 13);
  int dotSpacing = 14;
  int dotsW = totalDots * dotSpacing;
  int dotsX = (SCREEN_W - dotsW) / 2;
  tft.fillRect(dotsX - 4, barY + barH, dotsW + 8, 6, COL_BG);
  for (int i = 0; i < NUM_CATEGORIES; i++) {
    int dx = dotsX + i * dotSpacing + 5;
    if (i == selectedCategory)
      tft.fillCircle(dx, barY + barH + 3, 3, COL_YELL);
    else
      tft.drawCircle(dx, barY + barH + 3, 2, COL_MUTED);
  }
}

void drawLobby() {
  tft.fillScreen(COL_BG);

  // Bunter Header
  tft.fillRect(0, 0, SCREEN_W, 40, COL_HEAD);
  tft.fillRect(0, 0, SCREEN_W / 3, 40, COL_PURP);
  tft.fillRect(SCREEN_W * 2 / 3, 0, SCREEN_W / 3, 40, COL_ORNG);
  tft.setTextColor(COL_TEXT, COL_HEAD);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("Pub-Quiz", SCREEN_W / 2, 20);

  tft.setTextSize(1);
  tft.setTextColor(COL_MUTED, COL_BG);
  tft.setTextDatum(TL_DATUM);
  tft.drawString("WLAN: " AP_SSID "  |  " AP_IP_STR, 8, 48);

  // Spielerliste
  int col = 0, x = 8, y = 66;
  int colW = (SCREEN_W - 24) / 2;
  for (int i = 0; i < playerCount; i++) {
    tft.fillRoundRect(x, y, colW, 22, 4, COL_CARD);
    tft.setTextColor(COL_GREEN, COL_CARD);
    tft.setTextSize(1);
    tft.setTextDatum(ML_DATUM);
    tft.drawString("+ " + String(players[i].name), x + 8, y + 11);
    col++;
    if (col >= 2) { col = 0; x = 8; y += 26; }
    else x = 8 + colW + 8;
  }

  if (playerCount == 0) {
    tft.setTextColor(COL_MUTED, COL_BG);
    tft.setTextSize(1);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Warte auf Spieler...", SCREEN_W / 2, 110);
  }

  // Kategorie-Auswahl
  drawCategoryBar();

  // Start-Button
  if (playerCount > 0) {
    int bw = 160, bh = 36, bx = (SCREEN_W - bw) / 2, by = SCREEN_H - bh - 70;
    tft.fillRoundRect(bx, by, bw, bh, 8, COL_GREEN);
    tft.setTextColor(COL_BG, COL_GREEN);
    tft.setTextSize(2);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Starten", SCREEN_W / 2, by + bh / 2);
  }
}

void drawQuestionScreen() {
  tft.fillScreen(COL_BG);

  // Bunter Header
  tft.fillRect(0, 0, SCREEN_W, 36, COL_HEAD);
  tft.fillRect(0, 0, 40, 36, COL_PURP);
  tft.fillRect(SCREEN_W - 40, 0, 40, 36, COL_ORNG);

  tft.setTextColor(COL_TEXT, COL_HEAD);
  tft.setTextSize(2);
  tft.setTextDatum(ML_DATUM);
  tft.drawString("Frage " + String(currentQ + 1) + "/" + String(NUM_QUESTIONS_PER_GAME), 50, 18);

  int answered = 0;
  for (int i = 0; i < playerCount; i++)
    if (players[i].answerThisRound >= 0) answered++;
  tft.setTextSize(1);
  tft.setTextColor(COL_TEXT, COL_ORNG);
  tft.setTextDatum(MR_DATUM);
  tft.drawString(String(answered) + "/" + String(playerCount), SCREEN_W - 6, 18);

  // Frage gross
  tft.setTextColor(COL_TEXT, COL_BG);
  tft.setTextSize(2);
  tft.setTextDatum(TL_DATUM);
  drawWrapped(curQuestion(currentQ).text, 8, 46, SCREEN_W - 16, 22);

  // Hinweis unten
  tft.setTextColor(COL_MUTED, COL_BG);
  tft.setTextSize(1);
  tft.setTextDatum(BC_DATUM);
  tft.drawString("Antwort auf deinem Handy", SCREEN_W / 2, SCREEN_H - 4);
}

void drawResolutionScreen() {
  tft.fillScreen(COL_BG);

  // Bunter Header
  tft.fillRect(0, 0, SCREEN_W, 36, COL_HEAD);
  tft.fillRect(0, 0, 40, 36, COL_PURP);
  tft.fillRect(SCREEN_W - 40, 0, 40, 36, COL_ORNG);
  tft.setTextColor(COL_TEXT, COL_HEAD);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("Aufloesung", SCREEN_W / 2, 18);

  // Richtige Antwort
  int correct = curQuestion(currentQ).correct;
  const char* labels[] = {"A", "B", "C", "D"};
  tft.fillRoundRect(8, 44, SCREEN_W - 16, 34, 6, COL_GREEN);
  tft.setTextColor(COL_BG, COL_GREEN);
  tft.setTextSize(1);
  tft.setTextDatum(ML_DATUM);
  String ans = String(labels[correct]) + ": " + String(curQuestion(currentQ).answers[correct]);
  drawWrapped(ans.c_str(), 14, 52, SCREEN_W - 28, 13);

  // Wer hatte Recht - zweispaltig
  int y = 86;
  tft.setTextColor(COL_MUTED, COL_BG);
  tft.setTextSize(1);
  tft.setTextDatum(TL_DATUM);
  tft.drawString("Richtig:", 10, y); y += 14;

  bool any = false;
  int col = 0, cx = 18;
  int colW = (SCREEN_W - 36) / 2;
  for (int i = 0; i < playerCount; i++) {
    if (players[i].answerThisRound == correct) {
      tft.setTextColor(COL_GREEN, COL_BG);
      tft.drawString("+ " + String(players[i].name), cx, y);
      col++;
      if (col >= 2) { col = 0; cx = 18; y += 14; }
      else cx = 18 + colW;
      any = true;
    }
  }
  if (!any) {
    tft.setTextColor(COL_RED, COL_BG);
    tft.drawString("Niemand!", 18, y);
  }

  // Button unten - zentriert, feste Breite
  const char* btnTxt = (currentQ + 1 < NUM_QUESTIONS_PER_GAME) ? "Naechste Frage" : "Ergebnis";
  int bw = 220, bh = 44, bx = (SCREEN_W - bw) / 2, by = SCREEN_H - bh - 6;
  tft.fillRoundRect(bx, by, bw, bh, 8, COL_BLUE);
  tft.setTextColor(COL_TEXT, COL_BLUE);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(btnTxt, SCREEN_W / 2, by + bh / 2);
}

void drawResolutionTimer() {
  int remaining = max(0, (int)((RESOLUTION_TIME - (millis() - resolutionStart)) / 1000));
  // Zweistellig: 09, 08 ...
  char buf[4];
  sprintf(buf, "%02d", remaining);

  int bw = 220, bh = 44, bx = (SCREEN_W - bw) / 2, by = SCREEN_H - bh - 6;
  // Nur rechten Teil des Buttons neu zeichnen
  int timerX = bx + bw - 48;
  tft.fillRect(timerX, by + 2, 44, bh - 4, COL_BLUE);
  tft.setTextColor(COL_TEXT, COL_BLUE);
  tft.setTextSize(2);
  tft.setTextDatum(MR_DATUM);
  tft.drawString(String(buf), bx + bw - 6, by + bh / 2);
}

void drawResultsScreen() {
  tft.fillScreen(COL_BG);

  // Bunter Header
  tft.fillRect(0, 0, SCREEN_W, 36, COL_HEAD);
  tft.fillRect(0, 0, 40, 36, COL_PURP);
  tft.fillRect(SCREEN_W - 40, 0, 40, 36, COL_ORNG);
  tft.setTextColor(COL_TEXT, COL_HEAD);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("Pub-Quiz", SCREEN_W / 2, 18);

  // Sortieren
  Player sorted[MAX_PLAYERS];
  memcpy(sorted, players, sizeof(Player) * playerCount);
  for (int i = 0; i < playerCount - 1; i++)
    for (int j = 0; j < playerCount - i - 1; j++)
      if (sorted[j].score < sorted[j+1].score) {
        Player tmp = sorted[j]; sorted[j] = sorted[j+1]; sorted[j+1] = tmp;
      }

  // Rangliste zweispaltig
  const uint16_t medalCol[3] = {COL_YELL, COL_MUTED, 0xC8A0};
  int col = 0, x = 8, y = 44;
  int colW = (SCREEN_W - 24) / 2, rowH = 30;
  for (int i = 0; i < playerCount; i++) {
    uint16_t c = (i < 3) ? medalCol[i] : COL_CARD;
    tft.fillRoundRect(x, y, colW, rowH - 2, 4, c);
    tft.setTextColor(COL_BG, c);
    tft.setTextSize(1);
    tft.setTextDatum(ML_DATUM);
    tft.drawString(String(i+1) + ". " + String(sorted[i].name), x + 6, y + rowH/2 - 1);
    tft.setTextDatum(MR_DATUM);
    tft.drawString(String(sorted[i].score) + "P", x + colW - 6, y + rowH/2 - 1);
    col++;
    if (col >= 2) { col = 0; x = 8; y += rowH; }
    else x = 8 + colW + 8;
  }

  // Nochmal-Button unten zentriert
  int bw = 200, bh = 44, bx = (SCREEN_W - bw) / 2, by = SCREEN_H - bh - 6;
  tft.fillRoundRect(bx, by, bw, bh, 8, COL_GREEN);
  tft.setTextColor(COL_BG, COL_GREEN);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("Nochmal!", SCREEN_W / 2, by + bh / 2);
}

// ── Player HTML ──────────────────────────────────────────────────────────────
String buildHTML() {
  return R"rawhtml(<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1,user-scalable=no">
<title>Trivia</title>
<style>
*{box-sizing:border-box;margin:0;padding:0;-webkit-tap-highlight-color:transparent}
body{background:#111;color:#eee;font-family:-apple-system,sans-serif;height:100vh;display:flex;flex-direction:column}
#hdr{background:#1a1a2e;padding:12px 16px;display:flex;justify-content:space-between;align-items:center;border-bottom:1px solid #333}
#hdr h1{font-size:17px;color:#07f}
#hdrSub{font-size:11px;color:#555}
#main{flex:1;display:flex;flex-direction:column;align-items:center;justify-content:center;padding:16px;gap:16px}
/* Name */
#nameBox{width:100%;max-width:340px;text-align:center}
#nameBox h2{color:#07f;margin-bottom:8px}
#nameBox p{color:#888;font-size:13px;margin-bottom:20px}
#nameInp{width:100%;background:#222;border:1px solid #444;border-radius:10px;padding:14px;color:#eee;font-size:18px;outline:none;text-align:center;margin-bottom:14px}
#joinBtn{width:100%;background:#07f;border:none;border-radius:10px;padding:16px;color:#fff;font-size:18px;cursor:pointer}
/* Lobby */
#lobbyBox{text-align:center}
#lobbyBox h2{color:#07f;margin-bottom:10px}
#lobbyBox p{color:#888;font-size:14px}
/* Frage */
#qBox{width:100%}
#qInfo{text-align:center;color:#888;font-size:13px;margin-bottom:6px}
#timer{text-align:center;font-size:40px;font-weight:900;margin-bottom:14px}
.ok{color:#0f0}.warn{color:#fa0}.urgent{color:#f44}
.answers{display:grid;grid-template-columns:1fr 1fr;gap:12px}
.abtn{border:none;border-radius:12px;padding:0 10px;font-size:15px;font-weight:bold;color:#fff;cursor:pointer;display:flex;flex-direction:column;align-items:center;justify-content:center;gap:5px;height:100px}
.abtn:active{opacity:.7}
.abtn.sel{opacity:.45;border:3px solid #fff}
.abtn.dis{opacity:.35;cursor:default}
.albl{font-size:26px;font-weight:900}
.aa{background:#c44000}.ab{background:#004bcc}.ac{background:#007a28}.ad{background:#884400}
/* Warten */
#waitBox{text-align:center}
#waitBox h2{color:#888;margin-bottom:8px}
/* Ergebnis */
#resBox{width:100%;max-width:400px}
#resBox h2{color:#07f;text-align:center;margin-bottom:14px}
.rrow{display:flex;justify-content:space-between;padding:10px 14px;border-radius:8px;margin-bottom:8px;font-size:15px}
.r1{background:#3a3000;color:#ffd700}.r2{background:#2a2a2a;color:#ccc}.r3{background:#2a1500;color:#cd7f32}.rx{background:#1e1e1e;color:#eee}
</style>
</head>
<body>
<div id="hdr"><h1>CYD Trivia</h1><span id="hdrSub">...</span></div>
<div id="main">
  <div id="nameBox">
    <h2>Willkommen!</h2>
    <p>Gib deinen Namen ein</p>
    <input id="nameInp" type="text" placeholder="Dein Name" maxlength="20" autocorrect="off" autocomplete="off">
    <button id="joinBtn" onclick="join()">Mitspielen</button>
  </div>
  <div id="lobbyBox" style="display:none"><h2>Bereit!</h2><p>Warte auf Spielstart...</p></div>
  <div id="qBox" style="display:none">
    <div id="qInfo"></div>
    <div id="timer" class="ok">20</div>
    <div class="answers">
      <button class="abtn aa" onclick="ans(0)"><span class="albl">A</span><span id="tA"></span></button>
      <button class="abtn ab" onclick="ans(1)"><span class="albl">B</span><span id="tB"></span></button>
      <button class="abtn ac" onclick="ans(2)"><span class="albl">C</span><span id="tC"></span></button>
      <button class="abtn ad" onclick="ans(3)"><span class="albl">D</span><span id="tD"></span></button>
    </div>
  </div>
  <div id="waitBox" style="display:none"><h2 id="waitMsg">Antwort gesendet</h2><p>Warte auf die anderen...</p></div>
  <div id="resBox" style="display:none"><h2>Ergebnis</h2><div id="resList"></div></div>
</div>
<script>
let myName='', myAns=-1, lastState=-1, lastQ=-1, timerIv=null, tLeft=20;

function show(id){['nameBox','lobbyBox','qBox','waitBox','resBox'].forEach(s=>document.getElementById(s).style.display=s===id?'':'none');}

function join(){
  const n=document.getElementById('nameInp').value.trim();
  if(!n)return; myName=n;
  fetch('/join',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:'name='+encodeURIComponent(n)})
    .then(r=>r.text()).then(t=>{if(t==='ok'){show('lobbyBox');poll();}else alert(t);});
}

function ans(i){
  if(myAns>=0)return; myAns=i;
  document.querySelectorAll('.abtn').forEach((b,j)=>{b.classList.add(j===i?'sel':'dis');b.onclick=null;});
  fetch('/answer',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:'name='+encodeURIComponent(myName)+'&answer='+i});
  show('waitBox');
}

function startTimer(s){
  clearInterval(timerIv); tLeft=s; tick();
  timerIv=setInterval(()=>{tLeft--;tick();if(tLeft<=0)clearInterval(timerIv);},1000);
}
function tick(){
  const el=document.getElementById('timer');
  el.textContent=Math.max(0,tLeft);
  el.className=tLeft>10?'ok':tLeft>5?'warn':'urgent';
}

function esc(t){return t.replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;');}

function poll(){
  fetch('/state?name='+encodeURIComponent(myName)).then(r=>r.json()).then(d=>{
    document.getElementById('hdrSub').textContent=d.players+' Spieler | Frage '+d.q+'/'+d.total;
    if(d.state===0){
      if(lastState!==0){
        // Nochmal-Reset: Antworten zuruecksetzen
        myAns=-1; lastQ=-1;
        document.querySelectorAll('.abtn').forEach((b,i)=>{
          b.className='abtn '+['aa','ab','ac','ad'][i];
          b.onclick=()=>ans(i);
        });
      }
      show('lobbyBox');lastState=0;
    }
    else if(d.state===1){
      if(lastQ!==d.q){
        lastQ=d.q; myAns=-1;
        document.getElementById('qInfo').textContent='Frage '+d.q+' von '+d.total;
        ['A','B','C','D'].forEach((l,i)=>document.getElementById('t'+l).textContent=d['ans'+i]);
        document.querySelectorAll('.abtn').forEach((b,i)=>{
          b.className='abtn '+['aa','ab','ac','ad'][i];
          b.onclick=()=>ans(i);
        });
        startTimer(d.remaining); show('qBox');
      } else if(myAns>=0){show('waitBox');}
      lastState=1;
    }
    else if(d.state===2){clearInterval(timerIv);document.getElementById('waitMsg').textContent='Aufloesung...';show('waitBox');lastState=2;}
    else if(d.state===3){
      clearInterval(timerIv); show('resBox');
      const list=document.getElementById('resList'); list.innerHTML='';
      d.scores.forEach((s,i)=>{
        const div=document.createElement('div');
        div.className='rrow r'+(i<3?i+1:'x');
        div.innerHTML='<span>'+(i+1)+'. '+esc(s.name)+'</span><span>'+s.score+' Pkt</span>';
        list.appendChild(div);
      });
      lastState=3;
    }
  }).catch(()=>{}).finally(()=>setTimeout(poll,1500));
}

document.getElementById('nameInp').addEventListener('keydown',e=>{if(e.key==='Enter')join();});
</script>
</body>
</html>)rawhtml";
}

// ── Server Handler ────────────────────────────────────────────────────────────
void handleRoot()    { server.send(200,"text/html",buildHTML()); }

void handleCaptive() {
  // Spezielle Captive-Portal-Seite: leitet zum echten Browser weiter
  // Der Mini-Browser von iOS/Android zeigt diese Seite an
  String page = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
  page += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
  page += "<title>Trivia</title><style>";
  page += "*{box-sizing:border-box;margin:0;padding:0}";
  page += "body{background:#111;color:#eee;font-family:-apple-system,sans-serif;";
  page += "min-height:100vh;display:flex;flex-direction:column;align-items:center;";
  page += "justify-content:center;gap:20px;padding:24px;text-align:center}";
  page += "h1{color:#ffd700;font-size:26px}";
  page += "p{color:#888;font-size:14px;line-height:1.5}";
  page += "a{display:block;background:#07f;color:#fff;text-decoration:none;";
  page += "border-radius:12px;padding:16px 32px;font-size:18px;font-weight:bold;margin-top:8px}";
  page += "a:active{opacity:.8}";
  page += ".hint{font-size:12px;color:#555;margin-top:4px}";
  page += "</style></head><body>";
  page += "<h1>CYD Trivia</h1>";
  page += "<p>Du bist mit dem Trivia-WLAN verbunden.<br>Tippe den Button um das Spiel im Browser zu oeffnen.</p>";
  page += "<a href='http://" AP_IP_STR "'>Spiel starten</a>";
  page += "<p class='hint'>Falls der Button nicht funktioniert:<br>Oeffne <b>" AP_IP_STR "</b> im Browser</p>";
  page += "</body></html>";

  // Kein Redirect - Seite direkt anzeigen damit der Link tippbar ist
  server.send(200, "text/html", page);
}

void handleJoin() {
  if(!server.hasArg("name")){ server.send(400); return; }
  String name=server.arg("name"); name.trim();
  name.replace("<",""); name.replace(">",""); name.replace("\"","");
  if(name.length()==0){ server.send(400,"text/plain","Kein Name"); return; }
  if(gameState!=0){ server.send(200,"text/plain","Spiel laeuft bereits"); return; }
  uint32_t ip=server.client().remoteIP();
  if(findPlayer(ip)>=0){ server.send(200,"text/plain","ok"); return; }
  if(addPlayer(ip,name.c_str())<0){ server.send(200,"text/plain","Voll"); return; }
  drawLobby();
  server.send(200,"text/plain","ok");
}

void handleAnswer() {
  if(gameState!=1){ server.send(200,"text/plain","ok"); return; }
  if(!server.hasArg("answer")){ server.send(400); return; }
  uint32_t ip=server.client().remoteIP();
  int idx=findPlayer(ip);
  if(idx<0){ server.send(400,"text/plain","Nicht registriert"); return; }
  if(players[idx].answerThisRound>=0){ server.send(200,"text/plain","ok"); return; }
  int a=server.arg("answer").toInt();
  players[idx].answerThisRound=a;
  if(a==curQuestion(currentQ).correct) players[idx].score++;
  // Display-Status aktualisieren (nur Zaehler, kein Flackern)
  int answered=0;
  for(int i=0;i<playerCount;i++) if(players[i].answerThisRound>=0) answered++;
  // Kleiner Update-Bereich oben rechts
  tft.setTextSize(1); tft.setTextColor(COL_MUTED,COL_HEAD);
  tft.setTextDatum(MR_DATUM);
  tft.drawString(String(answered)+"/"+String(playerCount)+"  ", SCREEN_W-6, 18);
  server.send(200,"text/plain","ok");
}

void handleState() {
  int rem=max(0,(int)((QUESTION_TIME-(millis()-questionStart))/1000));
  String j="{\"state\":"+String(gameState)+",\"q\":"+String(currentQ+1)+
            ",\"total\":"+String(NUM_QUESTIONS_PER_GAME)+",\"players\":"+String(playerCount)+
            ",\"remaining\":"+String(rem);
  if(gameState==1){
    for(int i=0;i<4;i++)
      j+=",\"ans"+String(i)+"\":\""+String(curQuestion(currentQ).answers[i])+"\"";
  }
  if(gameState==3){
    Player s[MAX_PLAYERS]; memcpy(s,players,sizeof(Player)*playerCount);
    for(int i=0;i<playerCount-1;i++)
      for(int k=0;k<playerCount-i-1;k++)
        if(s[k].score<s[k+1].score){Player t=s[k];s[k]=s[k+1];s[k+1]=t;}
    j+=",\"scores\":[";
    for(int i=0;i<playerCount;i++){
      if(i>0)j+=",";
      j+="{\"name\":\""+String(s[i].name)+"\",\"score\":"+String(s[i].score)+"}";
    }
    j+="]";
  }
  j+="}";
  server.send(200,"application/json",j);
}

// ── Spielsteuerung ────────────────────────────────────────────────────────────
void startGame() {
  selectQuestions();
  currentQ=0;
  for(int i=0;i<playerCount;i++) players[i].score=0;
  resetAnswers();
  gameState=1;
  questionStart=millis();
  drawQuestionScreen();
}

void goToResolution() {
  gameState=2;
  resolutionStart=millis();
  drawResolutionScreen();
}

void goNext() {
  currentQ++;
  if(currentQ>=NUM_QUESTIONS_PER_GAME){
    gameState=3;
    drawResultsScreen();
  } else {
    resetAnswers();
    gameState=1;
    questionStart=millis();
    drawQuestionScreen();
  }
}

void resetGame() {
  gameState = 0;
  currentQ  = 0;
  // Spieler BEHALTEN - nur Score und Antworten zuruecksetzen
  for (int i = 0; i < playerCount; i++) {
    players[i].score = 0;
    players[i].answerThisRound = -1;
  }
  drawLobby();
}

// ── Setup & Loop ─────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  randomSeed(esp_random() ^ millis() ^ analogRead(0) ^ analogRead(1));
  buildAllPool();

  tft.begin();
  tft.setRotation(1);  // Querformat

  touchSPI.begin(XPT2046_CLK,XPT2046_MISO,XPT2046_MOSI,XPT2046_CS);
  ts.begin(touchSPI);
  ts.setRotation(1);  // Querformat

  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID);
  delay(500);
  IPAddress apIP(192,168,4,1);
  WiFi.softAPConfig(apIP,apIP,IPAddress(255,255,255,0));
  dns.start(53,"*",apIP);

  const char* hdrs[]={"User-Agent"};
  server.collectHeaders(hdrs,1);
  server.on("/",HTTP_GET,handleRoot);
  server.on("/join",HTTP_POST,handleJoin);
  server.on("/answer",HTTP_POST,handleAnswer);
  server.on("/state",HTTP_GET,handleState);
  server.on("/generate_204",handleCaptive);
  server.on("/fwlink",handleCaptive);
  server.on("/hotspot-detect.html",handleCaptive);
  server.on("/connecttest.txt",handleCaptive);
  server.onNotFound(handleCaptive);
  server.begin();

  drawLobby();
}

unsigned long lastTimerDraw=0;

void loop() {
  dns.processNextRequest();
  server.handleClient();

  // Frage: Zeit pruefen, alle geantwortet?
  if(gameState==1){
    if(millis()-questionStart>=QUESTION_TIME || allAnswered()){
      goToResolution();
    }
  }

  // Aufloesung: 10s Countdown, dann auto-weiter
  if(gameState==2){
    // Timer-Zahl alle Sekunde aktualisieren (kein Vollbild-Redraw -> kein Flackern)
    if(millis()-lastTimerDraw>1000){
      lastTimerDraw=millis();
      drawResolutionTimer();
    }
    if(millis()-resolutionStart>=RESOLUTION_TIME){
      goNext();
    }
  }

  // Touch - Rotation 1: V1 = p.x->screenX, p.y->screenY
  if(ts.tirqTouched()&&ts.touched()){
    if(!wasTouched){
      wasTouched=true;
      TS_Point p=ts.getPoint();
      int sx=map(p.x,200,3700,0,SCREEN_W);
      int sy=map(p.y,200,3700,0,SCREEN_H);

      if(gameState==0){
        int barY=SCREEN_H-56;
        // Linker Pfeil
        if(sx<44 && sy>barY){
          selectedCategory=(selectedCategory-1+NUM_CATEGORIES)%NUM_CATEGORIES;
          drawCategoryBar();
        }
        // Rechter Pfeil
        else if(sx>SCREEN_W-44 && sy>barY){
          selectedCategory=(selectedCategory+1)%NUM_CATEGORIES;
          drawCategoryBar();
        }
        // Start-Button
        else if(playerCount>0 && sy>SCREEN_H-108 && sy<SCREEN_H-72){
          startGame();
        }
      }
      else if(gameState==2&&sy>SCREEN_H-56) goNext();
      else if(gameState==3&&sy>SCREEN_H-56) resetGame();
    }
  } else {
    wasTouched=false;
  }

  delay(30);
}
