[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/H1vNwaly)

# Task Ventilator

## Ziel des Assignments

Ziel dieses Assignments war es, einen sogenannten "Task Ventilator" zu implementieren. Dieser "Ventilator" verteilt verschiedene Arbeitsaufgaben auf Worker-Prozesse und gibt anschließend die Gesamtarbeit der einzelnen Worker aus.

Da dieses Assignment lediglich zur Veranschaulichung diente, führen die Worker-Prozesse keine echte Arbeit aus, sondern "schlafen" für eine bestimmte Zeitspanne.

---

## Adressierte Themen

Das Hauptthema des Assignments waren **System-Calls**. Diese werden ständig und in großer Zahl von Programmen im Hintergrund aufgerufen, können jedoch auch direkt im eigenen Code verwendet werden. Dazu zählen unter anderem:

- Das Öffnen, Verwalten und Verwenden von File-Deskriptoren über `open`, `read` sowie die entsprechenden Message-Queue-Aufrufe `mq_send`, `mq_receive` …
- Das Reservieren und Freigeben von Speicher über `malloc` und `free`
- Das Starten, Beenden und Warten auf Prozesse bzw. Threads

Diese Teilbereiche wurden in der Aufgabe behandelt:

- **Message Queues** → Interprozesskommunikation  
- **Auslesen von Programmargumenten** mittels `getopt`  
- **Starten und Verwalten von Kindprozessen** mittels `fork` und `wait`

---

## Gelerntes

- Für C-Library-Funktionen, System-Calls sowie grundlegende Unix-Konzepte können die Man-Pages über `man <Befehl>` aufgerufen werden, um mehr über das jeweilige Thema zu erfahren.
- Primitive Libraries wie `getopt` erleichtern einfache Aufgaben wie das Parsen von Kommandozeilenargumenten.
- Prozesse können über spezielle "Dateien", sogenannte **Message Queues**, Daten austauschen.
- Ein Prozess kann über `fork` Kindprozesse erzeugen. `fork` liefert eine gültige Prozess-ID im Elternprozess zurück und `0` im Kindprozess. Dadurch kann mithilfe eines `if`-Branches unterschiedliche Logik in Eltern- und Kindprozess ausgeführt werden. Mit `wait`-Varianten kann später auf diese Kindprozesse gewartet werden.

---

## Anwendung in Bezug auf das Assignment

Die Worker-Prozesse werden mithilfe von `fork` gestartet und erhalten über eine **Message Queue** ihre Aufgaben. Diese bestehen lediglich aus einem `sleep`-Aufruf zur Simulation von Arbeit. Die Ergebnisse werden über eine **zweite Message Queue** an den Ventilator zurückgeschickt.

Für eine variable Anzahl an Worker-Prozessen, Aufgaben und Queue-Größe wurden **Kommandozeilenargumente** verwendet. Dabei kam die Standardbibliothek `getopt` zum Einsatz, um das Parsen zu erleichtern.

---

## Umsetzung

- Die Kindprozesse werden bewusst mit `waitpid` abgefragt, nachdem der Ventilator eine "Done Task"-Nachricht erhalten hat, um den Statuscode des Prozesses auslesen zu können.
- Es werden zwei getrennte Message Queues erstellt und verwendet: eine **Command Queue** und eine **Done Queue**. Der Ventilator sendet die Aufgaben über die Command Queue und empfängt die Ergebnisse über die Done Queue. Damit sind die Nachrichtenflüsse klar voneinander getrennt.
- Es werden keine speziellen Terminierungsnachrichten verschickt. Stattdessen gilt ein Arbeitspaket mit einem **Aufwandswert von 0** als Signal zur Terminierung.

---

## Schwierigkeiten

Die Synchronisation der Prozesse war relativ herausfordernd, da die gesamte Kommunikation ausschließlich über Message Queues erfolgte – selbst beim Debugging.

Durch ein paar unglückliche Tippfehler wurden die Resultat-Nachrichten fälschlicherweise über die **Command Queue** zurückgeschickt. Solche Fehler sind schwer zu erkennen, da Message Queues Binärdaten übertragen und nicht leicht eingesehen oder geprüft werden können.