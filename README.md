# Control de dos LED mediante comandos seriales con Arduino UNO

**Politécnico Grancolombiano · Internet de las Cosas · Entrega grupal 2 · Noviembre de 2026**

## Descripción

Este proyecto implementa un sistema de control para dos luces LED (verde y roja) mediante una placa **Arduino UNO R3**. El programa recibe comandos de un solo carácter por una comunicación serial configurada a **9600 baudios**, interpreta la instrucción y actualiza las salidas digitales.

La arquitectura propuesta para un montaje físico utiliza un módulo **Bluetooth HC-05** para recibir instrucciones desde un teléfono o una terminal compatible y enviarlas al Arduino mediante **UART**. En la práctica documentada en **Wokwi**, los comandos se introducen en el **monitor serial USB**: se prueba la interpretación de los datos y el control de los LED, **no** el enlace inalámbrico Bluetooth ni el emparejamiento del HC-05.

El código valida comandos incorrectos, genera respuestas `OK`, `ERROR` o `ERROR_FORMATO` y registra los eventos y estados en formato CSV. Para el muestreo de las salidas utiliza `millis()` en lugar de `delay()`, evitando detener el ciclo principal.

## Enlace de simulación

**[Abrir simulación en Wokwi] (https://wokwi.com/projects/481620457960530945)**

## Componentes

| Componente | Cantidad | Función |
| --- | ---: | --- |
| Arduino UNO R3 | 1 | Recibe comandos y controla las salidas digitales |
| LED verde | 1 | Representa el estado de la salida D2 |
| LED rojo | 1 | Representa el estado de la salida D3 |
| Resistencia de 220 Ω | 2 | Limita la corriente de cada LED |
| Protoboard | 1 | Facilita la organización del montaje |
| Cables de conexión | Varios | Interconectan los componentes |
| Módulo Bluetooth HC-05 | 1 (referencia física) | Recibiría comandos por Bluetooth y los entregaría por UART; no se emuló en Wokwi |

## Conexiones

### Circuito ejecutado en Wokwi

| Elemento | Conexión |
| --- | --- |
| LED verde | Arduino **D2 → ánodo (+)**; cátodo (−) → resistencia de **220 Ω → GND** |
| LED rojo | Arduino **D3 → ánodo (+)**; cátodo (−) → resistencia de **220 Ω → GND** |
| Recepción de comandos | **Monitor serial USB**, `9600` baudios y final de línea `LF` |

El circuito se define en [`Entregable - Semana 5`](diagram.json) y el programa se encuentra en [`Entregable - Semana 5`](sketch.ino).

### Integración de referencia con HC-05 (no validada en esta simulación)

El programa contempla una configuración alternativa con `SoftwareSerial`, activable mediante `#define USAR_HC05 1`. Para ella se reservan los pines **D10 (RX de Arduino)** y **D11 (TX de Arduino)**, manteniendo D2 y D3 para los LED.

| Pin del HC-05 | Conexión de referencia |
| --- | --- |
| VCC | Alimentación indicada para la placa del módulo (habitualmente 5 V en placas HC-05 con regulador) |
| GND | GND de Arduino |
| TXD | Arduino **D10 (RX)** |
| RXD | Arduino **D11 (TX)** mediante adaptación de nivel lógico cuando se requiera |

> El RXD del módulo suele trabajar a nivel lógico de 3,3 V; antes de realizar un montaje físico se deben comprobar las especificaciones de la placa HC-05 utilizada. Esta integración se describe como **referencia**, no como prueba realizada.

## Comandos disponibles

Los comandos son **sensibles a mayúsculas** y deben enviarse individualmente, terminados con `LF` (Enter).

| Comando | LED verde (D2) | LED rojo (D3) | Respuesta |
| --- | --- | --- | --- |
| `V` | Encendido | Apagado | `OK` |
| `R` | Apagado | Encendido | `OK` |
| `A` | Encendido | Encendido | `OK` |
| `0` | Apagado | Apagado | `OK` |
| `X` u otra letra no admitida | Sin cambios | Sin cambios | `ERROR` |
| Entrada vacía o con más de un carácter | Sin cambios | Sin cambios | `ERROR_FORMATO` |

**Importante:** a diferencia de otros ejemplos, esta versión **no acepta** `v`, `r` ni `a` minúsculas. En el registro original se observa que `v` fue rechazada.

## Cómo ejecutar y probar el proyecto

1. Abre el enlace público de Wokwi o crea un proyecto **Arduino UNO**.
2. Copia el contenido de [`Entregable - Semana 5`](sketch.ino) y [`Entregable - Semana 5`](diagram.json) en sus respectivos archivos del editor.
3. Comprueba que `#define USAR_HC05 0` permanezca configurado para Wokwi.
4. Haz clic en **Start Simulation**.
5. Abre el **monitor serial** y escribe, uno por uno, `V`, `R`, `A`, `0` y `X`. Pulsa **Enter** después de cada comando.
6. Observa los LED y las líneas `CMD` y `MUESTRA` que aparecen en el monitor serial.

La salida utiliza el formato:

```csv
evento,t_ms,origen,comando,verde,rojo,resultado
CMD,13358,USB,V,1,0,OK
CMD,85272,USB,R,0,1,OK
CMD,108898,USB,A,1,1,OK
CMD,142999,USB,0,0,0,OK
CMD,159717,USB,X,0,0,ERROR
```

## Pruebas realizadas y evidencias

En la ejecución aportada se registraron **cinco pruebas funcionales**, que cumplieron el comportamiento esperado. El comando `X` se considera correcto porque fue rechazado y no modificó las salidas.

| # | Comando | Resultado esperado | Estado registrado (verde, rojo) | Verificación | Evidencia |
| ---: | :---: | --- | :---: | --- | --- |
| 01 | `V` | Encender solo LED verde | `1, 0` | Correcto (`OK`) | [Captura V](Evidencias fotográficas//PRUEBA_V.png) |
| 02 | `R` | Encender solo LED rojo | `0, 1` | Correcto (`OK`) | [Captura R](Evidencias fotográficas//PRUEBA R.png) |
| 03 | `A` | Encender ambos LED | `1, 1` | Correcto (`OK`) | [Captura A](Evidencias fotográficas//PRUEBA A.png) |
| 04 | `0` | Apagar ambos LED | `0, 0` | Correcto (`OK`) | [Captura 0](Evidencias fotográficas//PRUEBA 0.png) |
| 05 | `X` | Rechazar comando inválido sin alterar LED | `0, 0` | Correcto (`ERROR`) | [Captura X](Evidencias fotográficas//PRUEBA X.png) |


## Estructura del repositorio

```text
Proyecto-ARDUINO/

├── Entregable - Semana 5/
│   ├── sketch.ino
│   ├── diagram.json
│   └── Control serial de dos LED - Arduino.pdf
├── Evidencias fotográficas/
│   ├── PRUEBA V.png
│   ├── PRUEBA R.png
│   ├── PRUEBA A.png
│   ├── PRUEBA 0.png
│   └── PRUEBA X.png
└── README.md

```

## Limitaciones: Bluetooth frente a UART simulado

- **Sin enlace inalámbrico real:** Wokwi no emula en esta práctica el emparejamiento, la radio ni la cobertura del HC-05. Las cinco pruebas se realizaron por el monitor serial USB.
- **UART verificada, Bluetooth pendiente:** se comprueba la lectura y validación de comandos y el estado de las salidas. No se ha medido conectividad inalámbrica, alcance, interferencias ni latencia de Bluetooth.
- **Niveles eléctricos no ensayados:** un montaje físico con HC-05 requiere revisar las tensiones admisibles en su entrada RXD y adaptar los niveles si corresponde.
- **Tiempos de simulación, no latencia:** la columna `t_ms` usa `millis()` y representa instantes internos de Arduino, pero no permite calcular el tiempo desde el envío del comando en un teléfono.
- **Una corrida registrada:** las muestras documentadas pertenecen a una sola ejecución; repetir la experiencia permitiría realizar análisis estadísticos más amplios.

## Documentación

El proceso de diseño, implementación, capturas, interpretación de resultados, discusión y bibliografía se desarrolla en el **Control serial de dos LED - Arduino**, incluido en la carpeta [`Entregable - Semana 5/`].

## Autores

**Politécnico Grancolombiano — Internet de las Cosas**

- **Julián Esteban Cepeda López**
- **Diego Alexander Betancur Marin**

## Enlaces del proyecto

- **Wokwi:**  https://wokwi.com/projects/481620457960530945
- **GitHub:** https://github.com/Julicepeda/Proyecto-ARDUINO

---
*Proyecto académico de la Entrega 2. La simulación documenta el control por comandos seriales; no se presenta como una comprobación de comunicación Bluetooth inalámbrica.*
