<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="es">
<context>
    <name>AOApplication</name>
    <message>
        <location filename="../../src/aoapplication.cpp" line="129"/>
        <source>Disconnected from server.</source>
        <translation>Desconectado del servidor.</translation>
    </message>
    <message>
        <location filename="../../src/aoapplication.cpp" line="152"/>
        <source>Error connecting to master server. Will try again in %1 seconds.</source>
        <translation>Error al conectarse a la lista de servidores. Se intentará nuevamente en %1 segundos.</translation>
    </message>
    <message>
        <location filename="../../src/aoapplication.cpp" line="157"/>
        <source>There was an error connecting to the master server.
We deploy multiple master servers to mitigate any possible downtime, but the client appears to have exhausted all possible methods of finding and connecting to one.
Please check your Internet connection and firewall, and please try again.</source>
        <translatorcomment>I translated master servers as &quot;lista de servidores&quot; because literally nobody will understand if i make the literal translation &quot;servidor maestro&quot;. And in the end a master server is just a list of servers. Also removed the part about having multiple master servers, i just don&apos;t think the average user will understand this even if i do a good translation.</translatorcomment>
        <translation>Hubo un error al obtener la lista de servidores. Verifique su conexión a Internet y firewall, y vuelva a intentarlo.</translation>
    </message>
    <message>
        <location filename="../../src/packet_distribution.cpp" line="94"/>
        <source>Outdated version! Your version: %1
Please go to aceattorneyonline.com to update.</source>
        <translation>¡Versión desactualizada! Su versión: %1
Ingrese a aceattorneyonline.com para actualizar.</translation>
    </message>
    <message>
        <source>You have been exiled from AO.
Have a nice day.</source>
        <translation type="obsolete">Has sido exiliado de AO.
Que tengas un buen día.</translation>
    </message>
    <message>
        <location filename="../../src/packet_distribution.cpp" line="238"/>
        <source>Attorney Online 2</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../../src/packet_distribution.cpp" line="266"/>
        <source>Loading</source>
        <translation>Cargando</translation>
    </message>
    <message>
        <location filename="../../src/packet_distribution.cpp" line="366"/>
        <source>Loading evidence:
%1/%2</source>
        <translation>Cargando evidencia:
%1/%2</translation>
    </message>
    <message>
        <location filename="../../src/packet_distribution.cpp" line="401"/>
        <location filename="../../src/packet_distribution.cpp" line="495"/>
        <source>Loading music:
%1/%2</source>
        <translation>Cargando música:
%1/%2</translation>
    </message>
    <message>
        <location filename="../../src/packet_distribution.cpp" line="319"/>
        <location filename="../../src/packet_distribution.cpp" line="468"/>
        <source>Loading chars:
%1/%2</source>
        <translation>Cargando personajes:
%1/%2</translation>
    </message>
    <message>
        <location filename="../../src/packet_distribution.cpp" line="677"/>
        <source>You have been kicked from the server.
Reason: %1</source>
        <translation>Has sido expulsado del servidor.
Razón: %1</translation>
    </message>
    <message>
        <location filename="../../src/packet_distribution.cpp" line="685"/>
        <source>You have been banned from the server.
Reason: %1</source>
        <translation>Has sido bloqueado de este servidor.
Razón: %1</translation>
    </message>
    <message>
        <location filename="../../src/packet_distribution.cpp" line="693"/>
        <source>You are banned on this server.
Reason: %1</source>
        <translation>Has sido bloqueado en este servidor.
Razón: %1</translation>
    </message>
    <message>
        <source>You have been kicked from the server.
Reason: </source>
        <translation type="obsolete">Has sido expulsado del servidor.
Razón: </translation>
    </message>
    <message>
        <source>You are banned on this server.
Reason: </source>
        <translation type="obsolete">Has sido bloqueado en este servidor.
Razón: </translation>
    </message>
</context>
<context>
    <name>AOCaseAnnouncerDialog</name>
    <message>
        <location filename="../../src/aocaseannouncerdialog.cpp" line="11"/>
        <source>Case Announcer</source>
        <translation>Anunciar caso</translation>
    </message>
    <message>
        <location filename="../../src/aocaseannouncerdialog.cpp" line="46"/>
        <source>Case title:</source>
        <translation>Título del caso:</translation>
    </message>
    <message>
        <location filename="../../src/aocaseannouncerdialog.cpp" line="56"/>
        <source>Defense needed</source>
        <translation>Se necesita abogado</translation>
    </message>
    <message>
        <location filename="../../src/aocaseannouncerdialog.cpp" line="58"/>
        <source>Prosecution needed</source>
        <translation>Se necesita fiscal</translation>
    </message>
    <message>
        <location filename="../../src/aocaseannouncerdialog.cpp" line="60"/>
        <source>Judge needed</source>
        <translation>Se necesita juez</translation>
    </message>
    <message>
        <location filename="../../src/aocaseannouncerdialog.cpp" line="62"/>
        <source>Jurors needed</source>
        <translation>Se necesita jurado</translation>
    </message>
    <message>
        <location filename="../../src/aocaseannouncerdialog.cpp" line="64"/>
        <source>Stenographer needed</source>
        <translation>Se necesita taquígrafo</translation>
    </message>
    <message>
        <source>Witness needed</source>
        <translation type="obsolete">Se necesita testigo</translation>
    </message>
</context>
<context>
    <name>AOOptionsDialog</name>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="12"/>
        <source>Settings</source>
        <translation>Ajustes</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="46"/>
        <source>Gameplay</source>
        <translation>Jugabilidad</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="62"/>
        <source>Theme:</source>
        <translation>Tema visual:</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="64"/>
        <source>Sets the theme used in-game. If the new theme changes the lobby&apos;s look as well, you&apos;ll need to reload the lobby for the changes to take effect, such as by joining a server and leaving it.</source>
        <translation>Establece el tema visual utilizado en el juego. Si el nuevo tema también cambia el aspecto del lobby, deberá volver a cargar el lobby para que los cambios surtan efecto, como unirse a un servidor y volver al lobby.</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="95"/>
        <source>Log goes downwards:</source>
        <translation>Invertir historial IC:</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="97"/>
        <source>If ticked, new messages will appear at the bottom (like the OOC chatlog). The traditional (AO1) behaviour is equivalent to this being unticked.</source>
        <translation>Si está marcado, los nuevos mensajes aparecerán en la parte inferior (como el chat OOC).</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="110"/>
        <source>Log length:</source>
        <translation>Limite del historial:</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="111"/>
        <source>The amount of messages the IC chatlog will keep before deleting older messages. A value of 0 or below counts as &apos;infinite&apos;.</source>
        <translation>La cantidad de mensajes que mantendrá el historial del chat IC antes de eliminar mensajes más antiguos. 0 significa &apos;infinito&apos;.</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="133"/>
        <source>Default username:</source>
        <translation>Usuario predeterminado:</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="135"/>
        <source>Your OOC name will be automatically set to this value when you join a server.</source>
        <translation>Su nombre OOC se establecerá automáticamente a este cuando se una a un servidor.</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="148"/>
        <source>Custom shownames:</source>
        <translation>Mostrar nombres:</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="150"/>
        <source>Gives the default value for the in-game &apos;Custom shownames&apos; tickbox, which in turn determines whether the client should display custom in-character names.</source>
        <translation>Activa la casilla &apos;Mostrar nombres&apos; de forma predeterminada en el juego, que a su vez determina si el cliente debe mostrar nombres personalizados en los personajes.</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="170"/>
        <source>Backup MS:</source>
        <translation>Master SV de respaldo:</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="172"/>
        <source>If the built-in server lookups fail, the game will try the address given here and use it as a backup master server address.</source>
        <translation>Si la lista de servidores predeterminada falla, el juego probará la dirección proporcionada aquí.</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="185"/>
        <source>Discord:</source>
        <translation>Discord:</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="187"/>
        <source>Allows others on Discord to see what server you are in, what character are you playing, and how long you have been playing for.</source>
        <translation>Permite a otros en Discord ver en qué servidor estás, qué personaje juegas y cuánto tiempo has estado jugando.</translation>
    </message>
    <message>
        <source>Allow Shake/Flash:</source>
        <translation type="obsolete">Permitir Shake/Flash:</translation>
    </message>
    <message>
        <source>Allows screenshaking and flashing. Disable this if you have concerns or issues with photosensitivity and/or seizures.</source>
        <translation type="obsolete">Permite el movimiento de la pantalla y el parpadeo. Desactive esto si tiene inquietudes o problemas con la fotosensibilidad y/o convulsiones.</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="200"/>
        <source>Language:</source>
        <translation>Idioma:</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="202"/>
        <source>Sets the language if you don&apos;t want to use your system language.</source>
        <translation>Establece el idioma si no desea utilizar el idioma de su sistema.</translation>
    </message>
    <message>
        <source>Punctuation delay modifier. Enable it for the blips to slow down on punctuations.</source>
        <translation type="obsolete">Habilítelo para agregar una pequeña pausa en los signos de puntuación.</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="338"/>
        <source>Callwords</source>
        <translation>Palabras clave</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="365"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;Enter as many callwords as you would like. These are case insensitive. Make sure to leave every callword in its own line!&lt;br&gt;Do not leave a line with a space at the end -- you will be alerted everytime someone uses a space in their messages.&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;Ingrese tantas palabras de llamada como desee.&lt;br&gt;Esto no distingue entre mayúsculas y minúsculas. ¡Asegúrese de dejar cada palabra en su propia línea!&lt;br&gt;No deje una línea con un espacio al final; recibirá una alerta cada vez que alguien use un espacio en sus mensajes.&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="375"/>
        <source>Audio</source>
        <translation>Audio</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="389"/>
        <source>Audio device:</source>
        <translation>Dispositivo:</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="390"/>
        <source>Sets the audio device for all sounds.</source>
        <translation>Establece el dispositivo de audio.</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="433"/>
        <source>Music:</source>
        <translation>Música:</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="434"/>
        <source>Sets the music&apos;s default volume.</source>
        <translation>Establece el volumen predeterminado de la música.</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="448"/>
        <source>SFX:</source>
        <translation>SFX:</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="450"/>
        <source>Sets the SFX&apos;s default volume. Interjections and actual sound effects count as &apos;SFX&apos;.</source>
        <translation>Establece el volumen predeterminado de SFX. Las interjecciones y los efectos de sonido reales cuentan como &apos;SFX&apos;.</translation>
    </message>
    <message>
        <source>Gives the default value for the in-game &apos;Custom shownames&apos; checkbox, which in turn determines whether the client should display custom in-character names.</source>
        <translation type="obsolete">Activa la casilla &apos;Mostrar nombres&apos; de forma predeterminada en el juego, que a su vez determina si el cliente debe mostrar nombres personalizados en los personajes.</translation>
    </message>
    <message>
        <source>Slower text speed:</source>
        <translation type="obsolete">Texto más lento:</translation>
    </message>
    <message>
        <source>Set the text speed to be the same as the AA games.</source>
        <translation type="obsolete">La velocidad del texto será la misma que en los juegos de AA.</translation>
    </message>
    <message>
        <source>Blip delay on punctuations:</source>
        <translation type="obsolete">Retraso en puntuación:</translation>
    </message>
    <message>
        <source>Sets the default volume for music.</source>
        <translation type="obsolete">Establece el volumen predeterminado de la música.</translation>
    </message>
    <message>
        <source>Sets the default volume for SFX sounds, like interjections or other character sound effects.</source>
        <translation type="obsolete">Establece el volumen predeterminado para sonidos SFX, como las interjecciones y otros efectos de sonido de personajes.</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="464"/>
        <source>Blips:</source>
        <translation>Blips:</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="466"/>
        <source>Sets the volume of the blips, the talking sound effects.</source>
        <translation>Establece el volumen de los blips, el sonido al hablar.</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="488"/>
        <source>Blip rate:</source>
        <translation>Tasa de blips:</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="490"/>
        <source>Sets the delay between playing the blip sounds.</source>
        <translation>Establece el retraso entre la reproducción de los sonidos blip.</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="505"/>
        <source>Blank blips:</source>
        <translation>Blips en blanco:</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="507"/>
        <source>If true, the game will play a blip sound even when a space is &apos;being said&apos;.</source>
        <translation>Si está marcada, el juego reproducirá un sonido blip incluso cuando se &apos;dice&apos; un espacio.</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="519"/>
        <source>Enable Looping SFX:</source>
        <translation>Habilitar repetición de SFX:</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="520"/>
        <source>If true, the game will allow looping sound effects to play on preanimations.</source>
        <translation>Si está habilitado, el juego permitirá que se reproduzcan efectos de sonido en bucle en preanimaciones.</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="532"/>
        <source>Kill Music On Objection:</source>
        <translation>Parar la música al objetar:</translation>
    </message>
    <message>
        <source>If true, the game will stop music when someone objects, like in the actual games.</source>
        <translation type="obsolete">Si está habilitado, el juego detendrá la música cuando alguien haga una objeción, como en los juegos.</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="208"/>
        <source> - Keep current setting</source>
        <translation> - Mantener la configuración actual</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="220"/>
        <source>Allow Screenshake:</source>
        <translation>Permitir screenshake:</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="222"/>
        <source>Allows screenshaking. Disable this if you have concerns or issues with photosensitivity and/or seizures.</source>
        <translation>Permite el movimiento de la pantalla (Advertencia esto podría inducir convulsiones debido a imágenes parpadeantes).</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="234"/>
        <source>Allow Effects:</source>
        <translation>Permitir efectos:</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="236"/>
        <source>Allows screen effects. Disable this if you have concerns or issues with photosensitivity and/or seizures.</source>
        <translation>Permite efectos de pantalla (Advertencia esto podría inducir convulsiones debido a imágenes parpadeantes).</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="248"/>
        <source>Network Frame Effects:</source>
        <translation>Enviar efectos al servidor:</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="249"/>
        <source>Send screen-shaking, flashes and sounds as defined in the char.ini over the network. Only works for servers that support this functionality.</source>
        <translation>Envíe temblores de pantalla, destellos y sonidos como se define en char.ini a través de la red. Solo funciona para servidores que admiten esta funcionalidad.</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="262"/>
        <source>Colors in IC Log:</source>
        <translation>Colores en el registro IC:</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="264"/>
        <source>Use the markup colors in the server IC chatlog.</source>
        <translation>Permite colores en el chat IC del servidor.</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="275"/>
        <source>Sticky Sounds:</source>
        <translation>Mantener sonidos:</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="277"/>
        <source>Turn this on to prevent the sound dropdown from clearing the sound after playing it.</source>
        <translation>Actívelo para evitar que el menú desplegable de sonido borre el sonido después de reproducirlo.</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="289"/>
        <source>Sticky Effects:</source>
        <translation>Mantener efectos:</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="291"/>
        <source>Turn this on to prevent the effects dropdown from clearing the effect after playing it.</source>
        <translation>Actívelo para evitar que el menú desplegable de efectos elimine el efecto después de reproducirlo.</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="304"/>
        <source>Sticky Preanims:</source>
        <translation>Mantener preanims:</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="306"/>
        <source>Turn this on to prevent preanimation checkbox from clearing after playing the emote.</source>
        <translation>Actívelo para evitar que la casilla preanimation se desactive después de reproducir el emote.</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="318"/>
        <source>Custom Chatboxes:</source>
        <translation>Chatboxes personalizados:</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="320"/>
        <source>Turn this on to allow characters to define their own custom chat box designs.</source>
        <translation>Actívelo para permitir que los personajes definan sus propios diseños de cuadros de chat personalizados.</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="498"/>
        <source>Play a blip sound &quot;once per every X symbols&quot;, where X is the blip rate.</source>
        <translation>Reproduce un sonido de blip &quot;una vez por cada X símbolos&quot;, donde X es la tasa de blip.</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="534"/>
        <source>If true, AO2 will stop the music for you when you or someone else does &apos;Objection!&apos;.</source>
        <translation>Si es activado, AO2 detendrá la música por ti cuando tú u otra persona hagan &apos;¡Objeción!&apos;.</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="546"/>
        <source>Casing</source>
        <translation>Caso</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="563"/>
        <source>This server supports case alerts.</source>
        <translation>Este servidor admite alertas de casos.</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="566"/>
        <source>This server does not support case alerts.</source>
        <translation>Este servidor no admite alertas de casos.</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="567"/>
        <source>Pretty self-explanatory.</source>
        <translation>Bastante autoexplicativo.</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="576"/>
        <source>Casing:</source>
        <translation>Caso:</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="578"/>
        <source>If checked, you will get alerts about case announcements.</source>
        <translation>Si está marcado, recibirá anuncios de casos.</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="594"/>
        <source>Defense:</source>
        <translation>Abogado:</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="595"/>
        <source>If checked, you will get alerts about case announcements if a defense spot is open.</source>
        <translation>Si está marcado, recibirá alertas sobre anuncios de casos si hay un lugar de abogado libre.</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="609"/>
        <source>Prosecution:</source>
        <translation>Fiscal:</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="611"/>
        <source>If checked, you will get alerts about case announcements if a prosecutor spot is open.</source>
        <translation>Si está marcada, recibirá alertas sobre anuncios de casos si hay un puesto de fiscal libre.</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="625"/>
        <source>Judge:</source>
        <translation>Juez:</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="626"/>
        <source>If checked, you will get alerts about case announcements if the judge spot is open.</source>
        <translation>Si está marcado, recibirá alertas sobre anuncios de casos si el puesto de juez está libre.</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="640"/>
        <source>Juror:</source>
        <translation>Jurado:</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="641"/>
        <source>If checked, you will get alerts about case announcements if a juror spot is open.</source>
        <translation>Si está marcado, recibirá alertas sobre anuncios de casos si hay un puesto de jurado libre.</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="655"/>
        <source>Stenographer:</source>
        <translation>Taquígrafo:</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="657"/>
        <source>If checked, you will get alerts about case announcements if a stenographer spot is open.</source>
        <translation>Si está marcado, recibirá alertas sobre anuncios de casos si hay un lugar de taquígrafo libre.</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="671"/>
        <source>CM:</source>
        <translation>CM:</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="673"/>
        <source>If checked, you will appear amongst the potential CMs on the server.</source>
        <translation>Si está marcado, aparecerá entre los posibles CM en el servidor.</translation>
    </message>
    <message>
        <source>Witness:</source>
        <translation type="obsolete">Testigo:</translation>
    </message>
    <message>
        <source>If checked, you will appear amongst the potential witnesses on the server.</source>
        <translation type="obsolete">Si está marcado, aparecerá entre los posibles testigos en el servidor.</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="687"/>
        <source>Hosting cases:</source>
        <translation>Casos:</translation>
    </message>
    <message>
        <location filename="../../src/aooptionsdialog.cpp" line="689"/>
        <source>If you&apos;re a CM, enter what cases you are willing to host.</source>
        <translation>Si eres un CM, ingresa qué casos estás dispuesto a organizar.</translation>
    </message>
    <message>
        <source>Only inline coloring will be shown such as &lt;&gt;,|| etc.</source>
        <translation type="obsolete">Solo se mostrará el color en línea, como &lt;&gt;, ||, etc.</translation>
    </message>
    <message>
        <source>IC log will mirror the IC box. Meaning that if somebody gets interrupted nobody will know what they wanted to say. Enable for a more realistic experience.</source>
        <translation type="obsolete">El registro de IC reflejará el chat In-Character. Lo que significa que si alguien es interrumpido, nadie sabrá lo que se iba a decir. Habilitar para una experiencia más realista.</translation>
    </message>
    <message>
        <source>IC log will mirror the IC box. Meaning that if somebody gets interrupted nobody will know what they wanted to say. Enable for a more realistic experience</source>
        <translation type="obsolete">El registro de IC reflejará el chat In-Character. Lo que significa que si alguien es interrumpido, nadie sabrá lo que se iba a decir. Habilitar para una experiencia más realista.</translation>
    </message>
    <message>
        <source>Colorful IC log:</source>
        <translation type="obsolete">Log IC colorido:</translation>
    </message>
    <message>
        <source>Enables colored text in the log.</source>
        <translation type="obsolete">Habilita texto con color en el log.</translation>
    </message>
    <message>
        <source>Only inline coloring:</source>
        <translation type="obsolete">Solo coloración en línea:</translation>
    </message>
    <message>
        <source>Only inline coloring will be shown such as &lt;&gt;,|| etc</source>
        <translation type="obsolete">Solo se mostrará el color en línea, como &lt;&gt;, ||, etc.</translation>
    </message>
    <message>
        <source>Mirror IC log:</source>
        <translation type="obsolete">IC log refleja interrupciones:</translation>
    </message>
    <message>
        <source>IC log will mirror the IC box. Meaning that if somebody gets interupted nobody will know what they wanted to say. Enable for a more realistic expierence</source>
        <translation type="obsolete">El registro de IC reflejará el chat In-Character. Lo que significa que si alguien es interrumpido, nadie sabrá lo que se iba a decir. Habilitar para una experiencia más realista.</translation>
    </message>
</context>
<context>
    <name>Courtroom</name>
    <message>
        <location filename="../../src/charselect.cpp" line="21"/>
        <source>Password</source>
        <translation>Contraseña</translation>
    </message>
    <message>
        <location filename="../../src/charselect.cpp" line="27"/>
        <source>Spectator</source>
        <translation>Espectador</translation>
    </message>
    <message>
        <location filename="../../src/charselect.cpp" line="30"/>
        <location filename="../../src/courtroom.cpp" line="175"/>
        <source>Search</source>
        <translation>Buscar</translation>
    </message>
    <message>
        <location filename="../../src/charselect.cpp" line="35"/>
        <source>Passworded</source>
        <translatorcomment>A translation wouldn&apos;t fit because of the shitty theme system.</translatorcomment>
        <translation></translation>
    </message>
    <message>
        <location filename="../../src/charselect.cpp" line="39"/>
        <source>Taken</source>
        <translation>En uso</translation>
    </message>
    <message>
        <source>Could not find %1</source>
        <translation type="obsolete">No se pudo encontrar %1</translation>
    </message>
    <message>
        <location filename="../../src/charselect.cpp" line="232"/>
        <source>Generating chars:
%1/%2</source>
        <translation>Generando personajes:
%1/%2</translation>
    </message>
    <message>
        <source>Generating chars:
</source>
        <translation type="obsolete">Generando personajes:
</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="148"/>
        <source>Showname</source>
        <translatorcomment>A translation wouldn&apos;t fit because of the shitty theme system.</translatorcomment>
        <translation></translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="152"/>
        <source>Message</source>
        <translation>Mensaje</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="167"/>
        <source>Name</source>
        <translation>Nombre</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="217"/>
        <source>Pre</source>
        <translatorcomment>A translation wouldn&apos;t fit because of the shitty theme system.</translatorcomment>
        <translation></translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="220"/>
        <source>Flip</source>
        <translatorcomment>A translation wouldn&apos;t fit because of the shitty theme system.</translatorcomment>
        <translation></translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="224"/>
        <source>Guard</source>
        <translation>Guardia</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="233"/>
        <location filename="../../src/courtroom.cpp" line="815"/>
        <source>Casing</source>
        <translatorcomment>This could be translated as &apos;caso&apos; and it wouldn&apos;t get cut, but there are so many other buttons that can&apos;t be translated on the courtroom window that might as well leave this also untranslated so it&apos;s at least consistent.</translatorcomment>
        <translation></translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="238"/>
        <source>Shownames</source>
        <translatorcomment>A translation wouldn&apos;t fit because of the shitty theme system.</translatorcomment>
        <translation></translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="241"/>
        <source>No Interrupt</source>
        <translatorcomment>A translation wouldn&apos;t fit because of the shitty theme system.</translatorcomment>
        <translation></translation>
    </message>
    <message>
        <source>White</source>
        <translation type="obsolete">Blanco</translation>
    </message>
    <message>
        <source>Green</source>
        <translation type="obsolete">Verde</translation>
    </message>
    <message>
        <source>Red</source>
        <translation type="obsolete">Rojo</translation>
    </message>
    <message>
        <source>Orange</source>
        <translation type="obsolete">Naranja</translation>
    </message>
    <message>
        <source>Blue</source>
        <translation type="obsolete">Azul</translation>
    </message>
    <message>
        <source>Yellow</source>
        <translation type="obsolete">Amarillo</translation>
    </message>
    <message>
        <source>Rainbow</source>
        <translation type="obsolete">Arcoíris</translation>
    </message>
    <message>
        <source>Pink</source>
        <translation type="obsolete">Rosado</translation>
    </message>
    <message>
        <source>Cyan</source>
        <translation type="obsolete">Cian</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="274"/>
        <source>% offset</source>
        <translation>% desplazamiento</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="742"/>
        <source>Music</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="744"/>
        <source>Sfx</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="746"/>
        <source>Blips</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="791"/>
        <source>Change character</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="797"/>
        <source>Reload theme</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="803"/>
        <source>Call mod</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="809"/>
        <source>Settings</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="822"/>
        <source>A/M</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="827"/>
        <source>Preanim</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="913"/>
        <source>Back to Lobby</source>
        <translatorcomment>&apos;Volver al lobby&apos; got cut, changed to just Lobby</translatorcomment>
        <translation>Lobby</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="1457"/>
        <source>You were granted the Disable Modcalls button.</source>
        <translation>Se le concedió el botón  para deshabilitar llamadas a moderadores.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="2994"/>
        <source>You have been banned.</source>
        <translation>Has sido vetado.</translation>
    </message>
    <message>
        <source>You were granted the Guard button.</source>
        <translation type="obsolete">Te ha sido otorgado el botón Guardia.</translation>
    </message>
    <message>
        <source>This does nothing, but there you go.</source>
        <translation type="obsolete">Esto no hace nada, pero ahí lo tienes.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="3194"/>
        <source>You opened the settings menu.</source>
        <translation>Abriste el menú de configuración.</translation>
    </message>
    <message>
        <source>You will now pair up with </source>
        <translation type="obsolete">Ahora te emparejarás con </translation>
    </message>
    <message>
        <source> if they also choose your character in return.</source>
        <translation type="obsolete"> si ellos también eligen a tu personaje a cambio.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="143"/>
        <source>None</source>
        <translation>Nada</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="228"/>
        <source>Additive</source>
        <translation>Aditivo</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="277"/>
        <source>To front</source>
        <translation>Al frente</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="278"/>
        <source>To behind</source>
        <translation>Al fondo</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="591"/>
        <source>Select a character you wish to pair with.</source>
        <translation>Seleccione un personaje con el que desee emparejarse.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="596"/>
        <source>Change the percentage offset of your character&apos;s position from the center of the screen.</source>
        <translation>Cambia el desplazamiento porcentual de la posición de tu personaje desde el centro de la pantalla.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="602"/>
        <source>Change the order of appearance for your character.</source>
        <translation>Cambia el orden de aparición de tu personaje.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="607"/>
        <source>Display the list of characters to pair with.</source>
        <translation>Muestra la lista de personajes para emparejar.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="665"/>
        <source>Oops, you&apos;re muted!</source>
        <translation>¡Ups, estas silenciado!</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="679"/>
        <source>Set your character&apos;s emote to play on your next message.</source>
        <translation>Configura el emote de tu personaje para usar en tu próximo mensaje.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="683"/>
        <source>Set your character&apos;s supplementary background.</source>
        <translation>Establece el fondo suplementario de tu personaje.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="689"/>
        <source>Set an &apos;iniswap&apos;, or an alternative character folder to refer to from your current character.
Edit by typing and pressing Enter, [X] to remove. This saves to your base/characters/&lt;charname&gt;/iniswaps.ini</source>
        <translation>Establece un &apos;iniswap&apos;, o una carpeta de caracteres alternativa para consultar desde su personaje actual.
Edite escribiendo y presionando Enter, [X] para eliminar. Esto es guardado en base/characters/&lt;charname&gt;/iniswaps.ini</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="698"/>
        <location filename="../../src/courtroom.cpp" line="715"/>
        <source>Remove the currently selected iniswap from the list and return to the original character folder.</source>
        <translation>Elimina el iniswap seleccionado actualmente de la lista y regresa a la carpeta de caracteres original.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="706"/>
        <source>Set a sound effect to play on your next &apos;Preanim&apos;. Leaving it on Default will use the emote-defined sound (if any).
Edit by typing and pressing Enter, [X] to remove. This saves to your base/characters/&lt;charname&gt;/soundlist.ini</source>
        <translation>Establece un efecto de sonido para jugar en su próximo &apos;Preanim&apos;. Dejarlo en Predeterminado usará el sonido definido por emoticones (si hay).
Edite escribiendo y presionando Enter, [X] para eliminar. Esto es guardado en base/characters/&lt;charname&gt;/iniswaps.ini</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="722"/>
        <source>Choose an effect to play on your next spoken message.
The effects are defined in your theme/effects/effects.ini. Your character can define custom effects by
char.ini [Options] category, effects = &apos;miscname&apos; where it referes to misc/&lt;miscname&gt;/effects.ini to read the effects.</source>
        <translation>Elije un efecto para reproducir en su próximo mensaje hablado.
Los efectos se definen en theme/effects/effects.ini. Tu personaje puede definir efectos personalizados por
char.ini [Opciones] categoría, effects = &apos;miscname&apos; donde se refiere a misc/&lt;miscname&gt;/effects.ini para leer los efectos.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="749"/>
        <source>Hold It!</source>
        <translation>¡Un Momento!</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="750"/>
        <location filename="../../src/courtroom.cpp" line="756"/>
        <location filename="../../src/courtroom.cpp" line="762"/>
        <source>When this is turned on, your next in-character message will be a shout!</source>
        <translatorcomment>Why the exclamation?</translatorcomment>
        <translation>Cuando esto es activado, tu próximo mensaje del personaje será un grito.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="755"/>
        <source>Objection!</source>
        <translation>¡Objeción!</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="761"/>
        <source>Take That!</source>
        <translation>¡Toma Eso!</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="769"/>
        <source>Toggle between server chat and global AO2 chat.</source>
        <translation>Alternar entre el chat del servidor y el chat global.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="773"/>
        <location filename="../../src/courtroom.cpp" line="777"/>
        <location filename="../../src/courtroom.cpp" line="783"/>
        <location filename="../../src/courtroom.cpp" line="787"/>
        <source>This will display the animation in the viewport as soon as it is pressed.</source>
        <translation>Esto mostrará la animación en el viewport tan pronto como se presione.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="781"/>
        <source>Guilty!</source>
        <translation>¡Culpable!</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="794"/>
        <source>Bring up the Character Select Screen and change your character.</source>
        <translation>Abre la pantalla de selección de personaje y cambia tu personaje.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="800"/>
        <source>Refresh the theme and update all of the ui elements to match.</source>
        <translation>Actualiza el tema y todos los elementos de la interfaz de usuario para que coincidan.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="806"/>
        <source>Request the attention of the current server&apos;s moderator.</source>
        <translation>Solicite la atención del moderador actual del servidor.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="812"/>
        <source>Allows you to change various aspects of the client.</source>
        <translation>Le permite cambiar varios aspectos del cliente.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="818"/>
        <source>An interface to help you announce a case (you have to be a CM first to be able to announce cases)</source>
        <translation>Una interfaz para ayudarlo a anunciar un caso (debe ser un CM para poder anunciar casos)</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="824"/>
        <source>Switch between Areas and Music lists</source>
        <translation>Cambiar entre áreas y listas de música</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="829"/>
        <source>Play a single-shot animation as defined by the emote when checked.</source>
        <translation>Reproduzca una animación de un solo disparo según lo definido por el emote cuando esté marcado.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="833"/>
        <source>If preanim is checked, display the input text immediately as the animation plays concurrently.</source>
        <translation>Si se marca preanim, muestre el texto de entrada inmediatamente mientras la animación se reproduce simultáneamente.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="837"/>
        <source>Mirror your character&apos;s emotes when checked.</source>
        <translation>Refleja los gestos de tu personaje cuando esté marcado.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="841"/>
        <source>Add text to your last spoken message when checked.</source>
        <translation>Agregue texto a su último mensaje hablado cuando esté marcado.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="845"/>
        <source>Do not listen to mod calls when checked, preventing them from playing sounds or focusing attention on the window.</source>
        <translation>No escucha llamadas de moderación cuando esté marcado, evitando que reproduzcan sonidos o centrando la atención en la ventana.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="849"/>
        <source>Lets you receive case alerts when enabled.
(You can set your preferences in the Settings!)</source>
        <translation>Le permite recibir alertas de casos cuando está habilitado.
(¡Puedes configurar tus preferencias en la Configuración!)</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="854"/>
        <source>Display customized shownames for all users when checked.</source>
        <translation>Mostrar nombres personalizados para todos los usuarios cuando esté marcado.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="857"/>
        <source>Custom Shout!</source>
        <translation>¡Grito personalizado!</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="860"/>
        <source>This will display the custom character-defined animation in the viewport as soon as it is pressed.
To make one, your character&apos;s folder must contain custom.[webp/apng/gif/png] and custom.[wav/ogg/opus] sound effect</source>
        <translation>Esto mostrará la animación de personaje personalizada definida en el viewport tan pronto como se presione.
Para hacer una, la carpeta de tu personaje debe contener efectos personalizados [webp/apng/gif/png]. y efectos [wav/ogg/opus] personalizados de sonido</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="868"/>
        <source>Play realization sound and animation in the viewport on the next spoken message when checked.</source>
        <translation>Reproduzca sonido y animación de realización en la ventana gráfica en el siguiente mensaje hablado cuando esté marcado.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="874"/>
        <source>Shake the screen on next spoken message when checked.</source>
        <translation>Agite la pantalla en el siguiente mensaje hablado cuando esté marcado.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="880"/>
        <source>Display the list of character folders you wish to mute.</source>
        <translation>Muestra la lista de carpetas de caracteres que desea silenciar.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="884"/>
        <location filename="../../src/courtroom.cpp" line="892"/>
        <source>Increase the health bar.</source>
        <translation>Aumenta la barra de salud.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="888"/>
        <location filename="../../src/courtroom.cpp" line="896"/>
        <source>Decrease the health bar.</source>
        <translation>Disminuye la barra de salud.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="900"/>
        <source>Change the text color of the spoken message.
You can also select a part of your currently typed message and use the dropdown to change its color!</source>
        <translation>Cambia el color del texto en el chat IC.
¡También puede seleccionar una parte de su mensaje escrito actualmente y usar el menú desplegable para cambiar su color!</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="914"/>
        <source>Return back to the server list.</source>
        <translation>Regresa a la lista de servidores.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="927"/>
        <source>Become a spectator. You won&apos;t be able to interact with the in-character screen.</source>
        <translation>Conviértete en espectador. No podrás interactuar con la pantalla del personaje.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="1457"/>
        <location filename="../../src/courtroom.cpp" line="3355"/>
        <source>CLIENT</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="3073"/>
        <location filename="../../src/courtroom.cpp" line="4491"/>
        <location filename="../../src/courtroom.cpp" line="4498"/>
        <source>has played a song</source>
        <translation>ha reproducido la canción</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="3207"/>
        <source>You will now pair up with %1 if they also choose your character in return.</source>
        <translation>Ahora se emparejará con %1 si también eligen a su personaje.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="3213"/>
        <source>You are no longer paired with anyone.</source>
        <translation>Ya no estás emparejado con nadie.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="3218"/>
        <source>Are you sure you typed that well? The char ID could not be recognised.</source>
        <translation>¿Estás seguro de que lo escribiste bien? El ID de personaje no pudo ser reconocido.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="3233"/>
        <source>You have set your offset to </source>
        <translation>Ha configurado su desplazamiento en </translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="3240"/>
        <source>Your offset must be between -100% and 100%!</source>
        <translation>¡Su desplazamiento debe estar entre -100% y 100%!</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="3245"/>
        <source>That offset does not look like one.</source>
        <translation>Ese desplazamiento no se parece a uno.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="3251"/>
        <source>You switched your music and area list.</source>
        <translation>Cambiaste tu lista de música y área.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="3258"/>
        <source>You have forcefully enabled features that the server may not support. You may not be able to talk IC, or worse, because of this.</source>
        <translation>Ha habilitado forzosamente funciones que el servidor puede no admitir. Es posible que no pueda hablar IC, o peor, debido a esto.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="3272"/>
        <source>Your pre-animations interrupt again.</source>
        <translation>Sus pre-animaciones interrumpen de nuevo.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="3275"/>
        <source>Your pre-animations will not interrupt text.</source>
        <translation>Sus pre-animaciones no interrumpirán el texto.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="3286"/>
        <source>Couldn&apos;t open chatlog.txt to write into.</source>
        <translation>No se pudo abrir chatlog.txt para escribir.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="3299"/>
        <source>The IC chatlog has been saved.</source>
        <translation>El chat IC se ha guardado.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="3312"/>
        <source>You don&apos;t have a `base/cases/` folder! It was just made for you, but seeing as it WAS just made for you, it&apos;s likely the case file you&apos;re looking for can&apos;t be found in there.</source>
        <translation>¡No tienes una carpeta `base/cases /`! Ha sido creada para ti. Pero debido a que no existia la carpeta, tampoco habían casos guardados ahí.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="3327"/>
        <source>You need to give a filename to load (extension not needed)! Make sure that it is in the `base/cases/` folder, and that it is a correctly formatted ini.
Cases you can load: %1</source>
        <translation>¡Debe dar un nombre de archivo para cargar (no se necesita extensión)! Asegúrese de que esté en la carpeta `base/cases/` y de que tenga el formato correcto.
Casos que puede cargar: %1</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="3355"/>
        <source>Case made by %1.</source>
        <translation>Caso hecho por %1.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="3365"/>
        <source>Navigate to %1 for the CM doc.</source>
        <translation>Navegue a %1 para el documento del CM.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="3379"/>
        <location filename="../../src/courtroom.cpp" line="3381"/>
        <location filename="../../src/evidence.cpp" line="761"/>
        <location filename="../../src/evidence.cpp" line="763"/>
        <source>UNKNOWN</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="3389"/>
        <source>Your case &quot;%1&quot; was loaded!</source>
        <translation>Su caso &quot;%1&quot; fue cargado!</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="3980"/>
        <source>Expand All Categories</source>
        <translation>Expandir todas las categorías</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="3982"/>
        <source>Collapse All Categories</source>
        <translation>Contraer todas las categorías</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="3986"/>
        <source>Fade Out Previous</source>
        <translation>Desvanecerse Anterior</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="3992"/>
        <source>Fade In</source>
        <translation>Fundirse</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="3998"/>
        <source>Synchronize</source>
        <translation>Sincronizar</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="4268"/>
        <source>Default</source>
        <translation>Predeterminado</translation>
    </message>
    <message>
        <source>You need to give a filename to load (extension not needed)! Make sure that it is in the `base/cases/` folder, and that it is a correctly formatted ini.
Cases you can load: </source>
        <translation type="obsolete">¡Debe dar un nombre de archivo para cargar (no se necesita extensión)! Asegúrese de que esté en la carpeta `base/cases/` y de que tenga el formato correcto.
Casos que puede cargar: </translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="3339"/>
        <source>Too many arguments to load a case! You only need one filename, without extension.</source>
        <translation>¡Demasiados argumentos para cargar un caso! Solo necesita un nombre de archivo, sin extensión.</translation>
    </message>
    <message>
        <source>Case made by </source>
        <translation type="obsolete">Caso hecho por </translation>
    </message>
    <message>
        <source>Navigate to </source>
        <translation type="obsolete">Navegue a </translation>
    </message>
    <message>
        <source> for the CM doc.</source>
        <translation type="obsolete"> para el documento de CM.</translation>
    </message>
    <message>
        <source>Your case &quot;</source>
        <translation type="obsolete">Su caso &quot;</translation>
    </message>
    <message>
        <source>&quot; was loaded!</source>
        <translation type="obsolete">&quot; fue cargado!</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="3401"/>
        <source>You don&apos;t have a `base/cases/` folder! It was just made for you, but seeing as it WAS just made for you, it&apos;s likely that you somehow deleted it.</source>
        <translation>¡No tienes una carpeta `base/cases /`! Fue creada para ti.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="3416"/>
        <source>You need to give a filename to save (extension not needed) and the courtroom status!</source>
        <translation>¡Debe dar un nombre de archivo para guardar (no se necesita la extensión) y el estado de la sala del tribunal!</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="3426"/>
        <source>Too many arguments to save a case! You only need a filename without extension and the courtroom status!</source>
        <translatorcomment>why two exclamations, seems excesive.</translatorcomment>
        <translation>¡Demasiados argumentos para salvar un caso! Solo necesita un nombre de archivo sin extensión y el estado de la sala del tribunal.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="3453"/>
        <source>Succesfully saved, edit doc and cmdoc link on the ini!</source>
        <translation>¡Guardado con éxito, puede editar el doc y doc link en el archivo ini!</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="3480"/>
        <source>Master</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="767"/>
        <location filename="../../src/courtroom.cpp" line="3487"/>
        <source>Server</source>
        <translation></translation>
    </message>
    <message>
        <source>OOC Message</source>
        <translation type="obsolete">Mensaje OOC</translation>
    </message>
    <message>
        <source>Too many arguments to save a case! You only need a filename without extension and the courtroom status.</source>
        <translation type="obsolete">¡Demasiados argumentos para salvar un caso! Solo necesita un nombre de archivo sin extensión y el estado de la sala del tribunal.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="4429"/>
        <source>Reason:</source>
        <translation>Razón:</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="4430"/>
        <source>Call Moderator</source>
        <translation>Llamar Moderador</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="4438"/>
        <location filename="../../src/courtroom.cpp" line="4442"/>
        <source>Error</source>
        <translation>Error</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="4438"/>
        <source>You must provide a reason.</source>
        <translation>Debes proporcionar una razón.</translation>
    </message>
    <message>
        <location filename="../../src/courtroom.cpp" line="4442"/>
        <source>The message is too long.</source>
        <translation>El mensaje es muy largo.</translation>
    </message>
    <message>
        <source>Choose...</source>
        <translation type="obsolete">Elegir...</translation>
    </message>
    <message>
        <location filename="../../src/evidence.cpp" line="17"/>
        <source>Present this piece of evidence to everyone on your next spoken message</source>
        <translation>Presente esta evidencia a todos en su próximo mensaje hablado</translation>
    </message>
    <message>
        <location filename="../../src/evidence.cpp" line="24"/>
        <source>Save evidence to an .ini file.</source>
        <translation>Guardar evidencia en un archivo .ini.</translation>
    </message>
    <message>
        <location filename="../../src/evidence.cpp" line="26"/>
        <source>Load evidence from an .ini file.</source>
        <translation>Cargar evidencia de un archivo .ini.</translation>
    </message>
    <message>
        <location filename="../../src/evidence.cpp" line="31"/>
        <source>Destroy this piece of evidence</source>
        <translation>Destruye esta evidencia</translation>
    </message>
    <message>
        <location filename="../../src/evidence.cpp" line="34"/>
        <source>Choose..</source>
        <translation>Elegir..</translation>
    </message>
    <message>
        <location filename="../../src/evidence.cpp" line="37"/>
        <source>Close the evidence display/editing overlay.
You will be prompted if there&apos;s any unsaved changes.</source>
        <translation>Cierra la superposición para visualizar/editar evidencia.
Se le preguntará si hay cambios no guardados.</translation>
    </message>
    <message>
        <location filename="../../src/evidence.cpp" line="40"/>
        <source>Save any changes made to this piece of evidence and send them to server.</source>
        <translation>Guardar los cambios realizados en esta evidencia y enviarlos al servidor.</translation>
    </message>
    <message>
        <location filename="../../src/evidence.cpp" line="48"/>
        <source>Double-click to edit. Press [X] to update your changes.</source>
        <translation>Doble click para editar. Presione [X] para actualizar sus cambios.</translation>
    </message>
    <message>
        <location filename="../../src/evidence.cpp" line="104"/>
        <source>Bring up the Evidence screen.</source>
        <translation>Abre la ventana para evidencia.</translation>
    </message>
    <message>
        <location filename="../../src/evidence.cpp" line="147"/>
        <source>Switch evidence to private inventory.</source>
        <translation>Cambiar la evidencia al inventario privado.</translation>
    </message>
    <message>
        <location filename="../../src/evidence.cpp" line="151"/>
        <source>Switch evidence to global inventory.</source>
        <translation>Cambiar la evidencia al inventario global.</translation>
    </message>
    <message>
        <location filename="../../src/evidence.cpp" line="158"/>
        <location filename="../../src/evidence.cpp" line="617"/>
        <source>Transfer evidence to private inventory.</source>
        <translation>Transferir evidencia al inventario privado.</translation>
    </message>
    <message>
        <location filename="../../src/evidence.cpp" line="163"/>
        <location filename="../../src/evidence.cpp" line="627"/>
        <source>Transfer evidence to global inventory.</source>
        <translation>Transferir evidencia al inventario global.</translation>
    </message>
    <message>
        <location filename="../../src/evidence.cpp" line="263"/>
        <source>The piece of evidence you&apos;ve been editing has changed.</source>
        <translation>La evidencia que has estado editando ha cambiado.</translation>
    </message>
    <message>
        <location filename="../../src/evidence.cpp" line="264"/>
        <source>Do you wish to keep your changes?</source>
        <translation>¿Desea conservar sus cambios?</translation>
    </message>
    <message>
        <location filename="../../src/evidence.cpp" line="265"/>
        <source>Name: %1
Image: %2
Description:
%3</source>
        <translation>Nombre: %1
Imagen: %2
Descripción:
%3</translation>
    </message>
    <message>
        <location filename="../../src/evidence.cpp" line="387"/>
        <source>Images (*.png)</source>
        <translation>Imágenes (* .png)</translation>
    </message>
    <message>
        <location filename="../../src/evidence.cpp" line="456"/>
        <location filename="../../src/evidence.cpp" line="460"/>
        <location filename="../../src/evidence.cpp" line="463"/>
        <source>Double-click to edit...</source>
        <translation>Doble click para editar...</translation>
    </message>
    <message>
        <location filename="../../src/evidence.cpp" line="478"/>
        <source>Add new evidence...</source>
        <translation>Añadir nueva evidencia...</translation>
    </message>
    <message>
        <location filename="../../src/evidence.cpp" line="556"/>
        <source>Evidence has been modified.</source>
        <translation>La evidencia ha sido modificada.</translation>
    </message>
    <message>
        <location filename="../../src/evidence.cpp" line="557"/>
        <source>Do you want to save your changes?</source>
        <translation>¿Quieres guardar tus cambios?</translation>
    </message>
    <message>
        <location filename="../../src/evidence.cpp" line="619"/>
        <source>Current evidence is global. Click to switch to private.</source>
        <translation>La evidencia actual es global. Haga clic para cambiar a privado.</translation>
    </message>
    <message>
        <location filename="../../src/evidence.cpp" line="629"/>
        <source>Current evidence is private. Click to switch to global.</source>
        <translation>La evidencia actual es privada. Haga clic para cambiar a global.</translation>
    </message>
    <message>
        <location filename="../../src/evidence.cpp" line="659"/>
        <source>&quot;%1&quot; has been transferred.</source>
        <translation>&quot;%1&quot; ha sido transferido.</translation>
    </message>
    <message>
        <location filename="../../src/evidence.cpp" line="721"/>
        <source>Save Inventory</source>
        <translation>Guardar inventario</translation>
    </message>
    <message>
        <location filename="../../src/evidence.cpp" line="721"/>
        <location filename="../../src/evidence.cpp" line="747"/>
        <source>Ini Files (*.ini)</source>
        <translation>Archivos Ini (*.ini)</translation>
    </message>
    <message>
        <location filename="../../src/evidence.cpp" line="747"/>
        <source>Open Inventory</source>
        <translation>Abrir el inventario</translation>
    </message>
</context>
<context>
    <name>Lobby</name>
    <message>
        <location filename="../../src/lobby.cpp" line="12"/>
        <source>Attorney Online 2</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../../src/lobby.cpp" line="31"/>
        <source>Search</source>
        <translation>Buscar</translation>
    </message>
    <message>
        <location filename="../../src/lobby.cpp" line="39"/>
        <source>Name</source>
        <translation>Nombre</translation>
    </message>
    <message>
        <location filename="../../src/lobby.cpp" line="94"/>
        <source>It doesn&apos;t look like your client is set up correctly.
Did you download all resources correctly from tiny.cc/getao, including the large &apos;base&apos; folder?</source>
        <translation>No parece que su cliente esté configurado correctamente.
¿Descargó todos los recursos correctamente desde tiny.cc/getao, incluida la gran carpeta &apos;base&apos;?</translation>
    </message>
    <message>
        <location filename="../../src/lobby.cpp" line="123"/>
        <source>Version: %1</source>
        <translation>Versión: %1</translation>
    </message>
    <message>
        <location filename="../../src/lobby.cpp" line="129"/>
        <source>Settings</source>
        <translation>Ajustes</translation>
    </message>
    <message>
        <location filename="../../src/lobby.cpp" line="132"/>
        <source>Allows you to change various aspects of the client.</source>
        <translation>Le permite cambiar varios aspectos del cliente.</translation>
    </message>
    <message>
        <location filename="../../src/lobby.cpp" line="176"/>
        <source>Loading</source>
        <translation>Cargando</translation>
    </message>
    <message>
        <location filename="../../src/lobby.cpp" line="180"/>
        <source>Cancel</source>
        <translation>Cancelar</translation>
    </message>
    <message>
        <location filename="../../src/lobby.cpp" line="365"/>
        <source>&lt;h2&gt;Attorney Online %1&lt;/h2&gt;The courtroom drama simulator&lt;p&gt;&lt;b&gt;Source code:&lt;/b&gt; &lt;a href=&apos;https://github.com/AttorneyOnline/AO2-Client&apos;&gt;https://github.com/AttorneyOnline/AO2-Client&lt;/a&gt;&lt;p&gt;&lt;b&gt;Major development:&lt;/b&gt;&lt;br&gt;OmniTroid, stonedDiscord, longbyte1, gameboyprinter, Cerapter, Crystalwarrior, Iamgoofball&lt;p&gt;&lt;b&gt;Client development:&lt;/b&gt;&lt;br&gt;Cents02, in1tiate, raidensnake, windrammer&lt;p&gt;&lt;b&gt;QA testing:&lt;/b&gt;&lt;br&gt;CaseyCazy, CedricDewitt, Chewable Tablets, CrazyJC, Fantos, Fury McFlurry, Geck, Gin-Gi, Jamania, Minx, Pandae, Robotic Overlord, Shadowlions (aka Shali), Sierra, SomeGuy, Veritas, Wiso&lt;p&gt;&lt;b&gt;Special thanks:&lt;/b&gt;&lt;br&gt;CrazyJC and MaximumVolty (2.8 release); Remy, Hibiki, court-records.net (sprites); Qubrick (webAO); Rue (website); Draxirch (UI design); Lewdton and Argoneus (tsuserver); Fiercy, Noevain, Cronnicossy, and FanatSors (AO1); server hosts, game masters, case makers, content creators, and the whole AO2 community!&lt;p&gt;The Attorney Online networked visual novel project is copyright (c) 2016-2020 Attorney Online developers. Open-source licenses apply. All other assets are the property of their respective owners.&lt;p&gt;Running on Qt version %2 with the %3 audio engine.&lt;p&gt;Built on %4</source>
        <translation>&lt;h2&gt;Attorney Online %1&lt;/h2&gt;El simulador de drama legal&lt;p&gt;&lt;b&gt;Código fuente:&lt;/b&gt; &lt;a href=&apos;https://github.com/AttorneyOnline/AO2-Client&apos;&gt;https://github.com/AttorneyOnline/AO2-Client&lt;/a&gt;&lt;p&gt;&lt;b&gt;Desarrollo mayor:&lt;/b&gt;&lt;br&gt;OmniTroid, stonedDiscord, longbyte1, gameboyprinter, Cerapter, Crystalwarrior, Iamgoofball&lt;p&gt;&lt;b&gt;Desarrollo del cliente:&lt;/b&gt;&lt;br&gt;Cents02, in1tiate, raidensnake, windrammer&lt;p&gt;&lt;b&gt;Prueba de control de calidad:&lt;/b&gt;&lt;br&gt;CaseyCazy, CedricDewitt, Chewable Tablets, CrazyJC, Fantos, Fury McFlurry, Geck, Gin-Gi, Jamania, Minx, Pandae, Robotic Overlord, Shadowlions (aka Shali), Sierra, SomeGuy, Veritas, Wiso&lt;p&gt;&lt;b&gt;Agradecimiento especial:&lt;/b&gt;&lt;br&gt;CrazyJC y MaximumVolty (versión 2.8); Remy, Hibiki, court-records.net (sprites); Qubrick (webAO); Rue (website); Draxirch (UI design); Lewdton and Argoneus (tsuserver); Fiercy, Noevain, Cronnicossy, y FanatSors (AO1); server hosts, game masters, case makers, creadores de contenido y toda la comunidad AO2.&lt;p&gt;El proyecto Attorney Online novela visual en red tiene copyright (c) 2016-2020 Attorney Online developers. Se aplican licencias de código abierto. Todos los demás activos son propiedad de sus respectivos dueños.&lt;p&gt;Usando Qt versión %2 con el motor de audio %3.&lt;p&gt;Compilado el %4</translation>
    </message>
    <message>
        <location filename="../../src/lobby.cpp" line="398"/>
        <source>About</source>
        <translation>Acerca de</translation>
    </message>
    <message>
        <source>&lt;h2&gt;Attorney Online %1&lt;/h2&gt;The courtroom drama simulator&lt;p&gt;&lt;b&gt;Source code:&lt;/b&gt; &lt;a href=&apos;https://github.com/AttorneyOnline/AO2-Client&apos;&gt;https://github.com/AttorneyOnline/AO2-Client&lt;/a&gt;&lt;p&gt;&lt;b&gt;Major development:&lt;/b&gt;&lt;br&gt;OmniTroid, stonedDiscord, longbyte1, gameboyprinter, Cerapter, Cents02&lt;p&gt;&lt;b&gt;Special thanks:&lt;/b&gt;&lt;br&gt;Remy, Iamgoofball, Hibiki, Qubrick (webAO), Ruekasu (UI design), Draxirch (UI design), Unishred, Argoneus (tsuserver), Fiercy, Noevain, Cronnicossy, Raidensnake</source>
        <translation type="obsolete">&lt;h2&gt;Attorney Online %1&lt;/h2&gt;The courtroom drama simulator&lt;p&gt;&lt;b&gt;Source code:&lt;/b&gt; &lt;a href=&apos;https://github.com/AttorneyOnline/AO2-Client&apos;&gt;https://github.com/AttorneyOnline/AO2-Client&lt;/a&gt;&lt;p&gt;&lt;b&gt;Major development:&lt;/b&gt;&lt;br&gt;OmniTroid, stonedDiscord, longbyte1, gameboyprinter, Cerapter, Cents02&lt;p&gt;&lt;b&gt;Special thanks:&lt;/b&gt;&lt;br&gt;Remy, Iamgoofball, Hibiki, Qubrick (webAO), Ruekasu (UI design), Draxirch (UI design), Unishred, Argoneus (tsuserver), Fiercy, Noevain, Cronnicossy, Raidensnake</translation>
    </message>
    <message>
        <source>&lt;h2&gt;Attorney Online %1&lt;/h2&gt;The courtroom drama simulator&lt;p&gt;&lt;b&gt;Source code:&lt;/b&gt; &lt;a href=&apos;https://github.com/AttorneyOnline/AO2-Client&apos;&gt;https://github.com/AttorneyOnline/AO2-Client&lt;/a&gt;&lt;p&gt;&lt;b&gt;Major development:&lt;/b&gt;&lt;br&gt;OmniTroid, stonedDiscord, longbyte1, gameboyprinter, Cerapter&lt;p&gt;&lt;b&gt;Special thanks:&lt;/b&gt;&lt;br&gt;Remy, Iamgoofball, Hibiki, Qubrick (webAO), Ruekasu (UI design), Draxirch (UI design), Unishred, Argoneus (tsuserver), Fiercy, Noevain, Cronnicossy</source>
        <translation type="obsolete">&lt;h2&gt;Attorney Online %1&lt;/h2&gt;El simulador de drama legal&lt;p&gt;&lt;b&gt;Código fuente:&lt;/b&gt; &lt;a href=&apos;https://github.com/AttorneyOnline/AO2-Client&apos;&gt;https: //github.com/AttorneyOnline/AO2-Client&lt;/a&gt;&lt;p&gt;&lt;b&gt;Desarrollo mayor:&lt;/b&gt; &lt;br&gt;OmniTroid, stonedDiscord, longbyte1, gameboyprinter, Cerapter&lt;p&gt;&lt;b&gt;Agradecimiento especial:&lt;/b&gt;&lt;br&gt;Remy, Iamgoofball, Hibiki, Qubrick (webAO), Ruekasu (diseño de interfaz de usuario), Draxirch (diseño de interfaz de usuario), Unishred, Argoneus (tsuserver), Fiercy, Noevain, Cronnicossy</translation>
    </message>
    <message>
        <location filename="../../src/lobby.cpp" line="540"/>
        <source>Online: %1/%2</source>
        <translation>En línea: %1/%2</translation>
    </message>
    <message>
        <location filename="../../src/lobby.cpp" line="142"/>
        <location filename="../../src/lobby.cpp" line="436"/>
        <source>Offline</source>
        <translation>Fuera de línea</translation>
    </message>
</context>
<context>
    <name>chatlogpiece</name>
    <message>
        <location filename="../../src/chatlogpiece.cpp" line="5"/>
        <location filename="../../src/chatlogpiece.cpp" line="6"/>
        <location filename="../../src/chatlogpiece.cpp" line="7"/>
        <source>UNKNOWN</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../../src/chatlogpiece.cpp" line="55"/>
        <source> has played a song: </source>
        <translation> ha comenzado a reproducir la canción: </translation>
    </message>
</context>
<context>
    <name>debug_functions</name>
    <message>
        <location filename="../../src/debug_functions.cpp" line="10"/>
        <source>Error: %1</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../../src/debug_functions.cpp" line="13"/>
        <source>Error</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../../src/debug_functions.cpp" line="25"/>
        <source>Notice</source>
        <translatorcomment>In spanish it would be &quot;Aviso&quot;, but I believe it&apos;s going to be more useful for bug reports to not translate any debug strings.</translatorcomment>
        <translation></translation>
    </message>
</context>
</TS>
