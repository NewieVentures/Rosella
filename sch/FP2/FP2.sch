EESchema Schematic File Version 4
LIBS:FP2-cache
EELAYER 29 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Faraday FP2"
Date "2018-10-18"
Rev "A"
Comp "Newie Ventures"
Comment1 "M Bright"
Comment2 "UTS"
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Connector_Generic:Conn_01x03 J1
U 1 1 5BB06A76
P 9250 3700
F 0 "J1" H 9200 4050 50  0000 L CNN
F 1 "DMX OUT" H 9050 3950 50  0000 L CNN
F 2 "TerminalBlock_Phoenix:TerminalBlock_Phoenix_MKDS-1,5-3-5.08_1x03_P5.08mm_Horizontal" H 9250 3700 50  0001 C CNN
F 3 "http://www.on-shore.com/wp-content/uploads/2015/09/OSTYCXX2150.pdf" H 9250 3700 50  0001 C CNN
F 4 "Digikey" H 9250 3700 50  0001 C CNN "Supplier"
F 5 "ED2903-ND" H 9250 3700 50  0001 C CNN "Order Number"
F 6 "On Shore Technology" H 9250 3700 50  0001 C CNN "Manufacturer"
F 7 "OSTYC032150" H 9250 3700 50  0001 C CNN "Part Number"
F 8 "TERMINAL BLOCK 5.08MM 3POS PCB" H 9250 3700 50  0001 C CNN "Description"
	1    9250 3700
	1    0    0    -1  
$EndComp
$Comp
L Interface_UART:MAX485E U1
U 1 1 5BB06A7D
P 6650 3600
F 0 "U1" H 6850 3000 50  0000 C CNN
F 1 "SP485E" H 6950 2900 50  0000 C CNN
F 2 "project_parts:SOIC-8_3.9x4.9mm_P1.27mm" H 6650 2900 50  0001 C CNN
F 3 "https://www.exar.com/ds/sp481e_sp485e.pdf" H 6650 3650 50  0001 C CNN
F 4 "SP485EEN-L/TR" H 6650 3600 50  0001 C CNN "Part Number"
F 5 "MaxLinear" H 6650 3600 50  0001 C CNN "Manufacturer"
F 6 "IC TXRX RS485 HALF DUPLEX 8SOIC " H 6650 3600 50  0001 C CNN "Description"
F 7 "Digikey" H 6650 3600 50  0001 C CNN "Supplier"
F 8 "1016-1827-6-ND" H 6650 3600 50  0001 C CNN "Order Number"
	1    6650 3600
	1    0    0    -1  
$EndComp
$Comp
L Device:R R2
U 1 1 5BB06A8D
P 7300 3500
F 0 "R2" V 7200 3450 50  0000 C CNN
F 1 "10R" V 7200 3650 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 7230 3500 50  0001 C CNN
F 3 "https://www.rohm.com/datasheet/ESR01MZPF/esr-e" H 7300 3500 50  0001 C CNN
F 4 "RES SMD 10 OHM 5% 0.4W 0805" V 7300 3500 50  0001 C CNN "Description"
F 5 "Digikey" V 7300 3500 50  0001 C CNN "Supplier"
F 6 "ESR10EZPJ100" V 7300 3500 50  0001 C CNN "Part Number"
F 7 "RHM10KCT-ND" V 7300 3500 50  0001 C CNN "Order Number"
F 8 "Rohm" V 7300 3500 50  0001 C CNN "Manufacturer"
	1    7300 3500
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0101
U 1 1 5BB06A94
P 7900 4700
F 0 "#PWR0101" H 7900 4450 50  0001 C CNN
F 1 "GND" H 7905 4527 50  0000 C CNN
F 2 "" H 7900 4700 50  0001 C CNN
F 3 "" H 7900 4700 50  0001 C CNN
	1    7900 4700
	1    0    0    -1  
$EndComp
$Comp
L Device:D_TVS_ALT D1
U 1 1 5BB06A9B
P 7800 4400
F 0 "D1" V 7750 4150 50  0000 L CNN
F 1 "43V 600W" V 7850 3900 50  0000 L CNN
F 2 "project_parts:D_SMB_NP" H 7800 4400 50  0001 C CNN
F 3 "http://www.littelfuse.com/~/media/electronics/datasheets/tvs_diodes/littelfuse_tvs_diode_smbj_datasheet.pdf.pdf" H 7800 4400 50  0001 C CNN
F 4 "TVS DIODE 43V 69.4V DO214AA" V 7800 4400 50  0001 C CNN "Description"
F 5 "SMBJ43CA" V 7800 4400 50  0001 C CNN "Part Number"
F 6 "Digikey" V 7800 4400 50  0001 C CNN "Supplier"
F 7 "Littelfuse" V 7800 4400 50  0001 C CNN "Manufacturer"
F 8 "SMBJ43CALFCT-ND" V 7800 4400 50  0001 C CNN "Order Number"
	1    7800 4400
	0    1    1    0   
$EndComp
Wire Wire Line
	7800 4550 7800 4650
Wire Wire Line
	7800 4650 7900 4650
Wire Wire Line
	8000 4650 7900 4650
Connection ~ 7900 4650
Wire Wire Line
	8000 4650 8000 4550
Wire Wire Line
	7900 4650 7900 4700
Wire Wire Line
	7800 4250 7800 3800
Wire Wire Line
	7800 3800 7450 3800
Wire Wire Line
	8000 4250 8000 3500
Wire Wire Line
	8000 3500 7450 3500
Connection ~ 8000 3500
Connection ~ 7800 3800
$Comp
L power:GND #PWR0102
U 1 1 5BB06AAE
P 9000 3900
F 0 "#PWR0102" H 9000 3650 50  0001 C CNN
F 1 "GND" H 9005 3727 50  0000 C CNN
F 2 "" H 9000 3900 50  0001 C CNN
F 3 "" H 9000 3900 50  0001 C CNN
	1    9000 3900
	1    0    0    -1  
$EndComp
Wire Wire Line
	9000 3900 9000 3800
Wire Wire Line
	7150 3500 7050 3500
Wire Wire Line
	7150 3800 7050 3800
$Comp
L power:GND #PWR0103
U 1 1 5BB06AB7
P 6650 4300
F 0 "#PWR0103" H 6650 4050 50  0001 C CNN
F 1 "GND" H 6655 4127 50  0000 C CNN
F 2 "" H 6650 4300 50  0001 C CNN
F 3 "" H 6650 4300 50  0001 C CNN
	1    6650 4300
	1    0    0    -1  
$EndComp
Wire Wire Line
	6650 4300 6650 4200
$Comp
L power:+5V #PWR0104
U 1 1 5BB06ABE
P 6650 3000
F 0 "#PWR0104" H 6650 2850 50  0001 C CNN
F 1 "+5V" H 6665 3173 50  0000 C CNN
F 2 "" H 6650 3000 50  0001 C CNN
F 3 "" H 6650 3000 50  0001 C CNN
	1    6650 3000
	1    0    0    -1  
$EndComp
Wire Wire Line
	6650 3100 6650 3000
$Comp
L Device:R R3
U 1 1 5BB06ACE
P 8200 3650
F 0 "R3" H 8050 3600 50  0000 C CNN
F 1 "120R" H 8000 3700 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 8130 3650 50  0001 C CNN
F 3 "https://www.rohm.com/datasheet/ESR01MZPF/esr-e" H 8200 3650 50  0001 C CNN
F 4 "RES SMD 120 OHM 5% 0.4W 0805" V 8200 3650 50  0001 C CNN "Description"
F 5 "ESR10EZPJ121" H 8200 3650 50  0001 C CNN "Part Number"
F 6 "Rohm" H 8200 3650 50  0001 C CNN "Manufacturer"
F 7 "Digikey" H 8200 3650 50  0001 C CNN "Supplier"
F 8 "RHM120KCT-ND" H 8200 3650 50  0001 C CNN "Order Number"
	1    8200 3650
	-1   0    0    1   
$EndComp
Wire Wire Line
	7800 3800 8200 3800
Wire Wire Line
	8000 3500 8200 3500
Connection ~ 8200 3500
Connection ~ 8200 3800
Text Label 4700 3700 0    50   ~ 0
RS485_DRV_EN
Text Label 4700 3600 0    50   ~ 0
RS485_RCV_EN
Wire Wire Line
	8700 3500 8700 3700
Wire Wire Line
	8200 3800 8850 3800
Wire Wire Line
	8200 3500 8700 3500
Wire Wire Line
	8850 3800 8850 3600
Text Notes 9350 3650 0    50   ~ 0
A
Text Notes 9350 3750 0    50   ~ 0
B
Text Notes 9350 3850 0    50   ~ 0
G
Wire Wire Line
	9000 3800 9050 3800
Wire Wire Line
	8700 3700 9050 3700
Wire Wire Line
	8850 3600 9050 3600
$Comp
L Device:R R1
U 1 1 5BB06AE9
P 5350 3250
F 0 "R1" H 5200 3200 50  0000 C CNN
F 1 "10k" H 5200 3300 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 5280 3250 50  0001 C CNN
F 3 "http://www.yageo.com/documents/recent/PYu-RC_Group_51_RoHS_L_9.pdf" H 5350 3250 50  0001 C CNN
F 4 "RES SMD 10K OHM 1% 1/8W 0805" V 5350 3250 50  0001 C CNN "Description"
F 5 "Yageo" H 5350 3250 50  0001 C CNN "Manufacturer"
F 6 "RC0805FR-0710KL" H 5350 3250 50  0001 C CNN "Part Number"
F 7 "311-10.0KCRCT-ND" H 5350 3250 50  0001 C CNN "Order Number"
F 8 "Digikey" H 5350 3250 50  0001 C CNN "Supplier"
	1    5350 3250
	-1   0    0    1   
$EndComp
$Comp
L power:+5V #PWR0105
U 1 1 5BB06AF0
P 5350 3000
F 0 "#PWR0105" H 5350 2850 50  0001 C CNN
F 1 "+5V" H 5365 3173 50  0000 C CNN
F 2 "" H 5350 3000 50  0001 C CNN
F 3 "" H 5350 3000 50  0001 C CNN
	1    5350 3000
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 3400 5350 3500
Wire Wire Line
	5350 3100 5350 3000
$Comp
L power:GND #PWR0106
U 1 1 5BB06B0B
P 5450 4300
F 0 "#PWR0106" H 5450 4050 50  0001 C CNN
F 1 "GND" H 5455 4127 50  0000 C CNN
F 2 "" H 5450 4300 50  0001 C CNN
F 3 "" H 5450 4300 50  0001 C CNN
	1    5450 4300
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 4200 5350 4300
Wire Wire Line
	5350 4300 5450 4300
Wire Wire Line
	5550 4300 5550 4200
Connection ~ 5450 4300
Wire Wire Line
	5450 4300 5550 4300
Wire Wire Line
	5550 3900 5550 3700
Wire Wire Line
	5350 3900 5350 3600
Wire Wire Line
	5550 3700 6250 3700
Wire Wire Line
	5350 3600 6250 3600
Wire Wire Line
	4700 3800 6250 3800
Wire Wire Line
	4700 3500 5350 3500
Connection ~ 5350 3500
Wire Wire Line
	5350 3500 6250 3500
Text Label 1300 3950 0    50   ~ 0
UART_TX
Text Label 1300 4050 0    50   ~ 0
UART_RX
Wire Wire Line
	1300 3950 1700 3950
Wire Wire Line
	1300 4050 1700 4050
Text Label 4700 3800 0    50   ~ 0
UART_TX
Text Label 4700 3500 0    50   ~ 0
UART_RX
Text Label 3350 4100 2    50   ~ 0
RS485_DRV_EN
Text Label 3350 4000 2    50   ~ 0
RS485_RCV_EN
Wire Wire Line
	3350 4000 2700 4000
Wire Wire Line
	3350 4100 2700 4100
$Comp
L power:+5V #PWR0107
U 1 1 5BAE26B1
P 1600 3300
F 0 "#PWR0107" H 1600 3150 50  0001 C CNN
F 1 "+5V" H 1615 3473 50  0000 C CNN
F 2 "" H 1600 3300 50  0001 C CNN
F 3 "" H 1600 3300 50  0001 C CNN
	1    1600 3300
	1    0    0    -1  
$EndComp
Wire Wire Line
	1700 3400 1600 3400
Wire Wire Line
	1600 3400 1600 3300
$Comp
L power:GND #PWR0108
U 1 1 5BAE377C
P 1600 4300
F 0 "#PWR0108" H 1600 4050 50  0001 C CNN
F 1 "GND" H 1605 4127 50  0000 C CNN
F 2 "" H 1600 4300 50  0001 C CNN
F 3 "" H 1600 4300 50  0001 C CNN
	1    1600 4300
	1    0    0    -1  
$EndComp
Wire Wire Line
	1600 4300 1600 4200
Wire Wire Line
	1600 4200 1700 4200
$Comp
L power:GND #PWR0109
U 1 1 5BAE48DE
P 2800 4300
F 0 "#PWR0109" H 2800 4050 50  0001 C CNN
F 1 "GND" H 2805 4127 50  0000 C CNN
F 2 "" H 2800 4300 50  0001 C CNN
F 3 "" H 2800 4300 50  0001 C CNN
	1    2800 4300
	1    0    0    -1  
$EndComp
Wire Wire Line
	2700 4200 2800 4200
Wire Wire Line
	2800 4200 2800 4300
NoConn ~ 2700 3400
NoConn ~ 1700 3550
NoConn ~ 1700 3650
NoConn ~ 1700 3750
NoConn ~ 1700 3850
NoConn ~ 2700 3500
NoConn ~ 2700 3600
NoConn ~ 2700 3700
NoConn ~ 2700 3800
NoConn ~ 2700 3900
Wire Wire Line
	7050 2300 7050 2250
Wire Wire Line
	7050 2650 7050 2600
Wire Wire Line
	7350 2300 7350 2250
Wire Wire Line
	7350 2650 7350 2600
Wire Wire Line
	7200 2650 7050 2650
Wire Wire Line
	7350 2650 7200 2650
Connection ~ 7200 2650
$Comp
L power:GND #PWR0110
U 1 1 5BB06B2A
P 7200 2650
F 0 "#PWR0110" H 7200 2400 50  0001 C CNN
F 1 "GND" H 7205 2477 50  0000 C CNN
F 2 "" H 7200 2650 50  0001 C CNN
F 3 "" H 7200 2650 50  0001 C CNN
	1    7200 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	7200 2250 7350 2250
Wire Wire Line
	7050 2250 7200 2250
Connection ~ 7200 2250
$Comp
L power:+5V #PWR0111
U 1 1 5BB06B24
P 7200 2250
F 0 "#PWR0111" H 7200 2100 50  0001 C CNN
F 1 "+5V" H 7215 2423 50  0000 C CNN
F 2 "" H 7200 2250 50  0001 C CNN
F 3 "" H 7200 2250 50  0001 C CNN
	1    7200 2250
	1    0    0    -1  
$EndComp
$Comp
L Device:C C2
U 1 1 5BB06B1D
P 7350 2450
F 0 "C2" H 7465 2496 50  0000 L CNN
F 1 "100nF 50V" H 7465 2405 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 7388 2300 50  0001 C CNN
F 3 "https://psearch.en.murata.com/capacitor/product/GCM21BR71H104KA37%23.pdf" H 7350 2450 50  0001 C CNN
F 4 "CAP CER 0.1UF 50V X7R 0805" H 7350 2450 50  0001 C CNN "Description"
F 5 "GCM21BR71H104KA37K" H 7350 2450 50  0001 C CNN "Part Number"
F 6 "490-8049-1-ND" H 7350 2450 50  0001 C CNN "Order Number"
F 7 "Digikey" H 7350 2450 50  0001 C CNN "Supplier"
F 8 "Murata" H 7350 2450 50  0001 C CNN "Manufacturer"
	1    7350 2450
	1    0    0    -1  
$EndComp
$Comp
L Device:R R6
U 1 1 5BB34519
P 5550 4050
F 0 "R6" H 5400 4000 50  0000 C CNN
F 1 "10k" H 5400 4100 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 5480 4050 50  0001 C CNN
F 3 "http://www.yageo.com/documents/recent/PYu-RC_Group_51_RoHS_L_9.pdf" H 5550 4050 50  0001 C CNN
F 4 "RES SMD 10K OHM 1% 1/8W 0805" V 5550 4050 50  0001 C CNN "Description"
F 5 "Yageo" H 5550 4050 50  0001 C CNN "Manufacturer"
F 6 "RC0805FR-0710KL" H 5550 4050 50  0001 C CNN "Part Number"
F 7 "311-10.0KCRCT-ND" H 5550 4050 50  0001 C CNN "Order Number"
F 8 "Digikey" H 5550 4050 50  0001 C CNN "Supplier"
	1    5550 4050
	-1   0    0    1   
$EndComp
$Comp
L Device:R R5
U 1 1 5BB34577
P 5350 4050
F 0 "R5" H 5200 4000 50  0000 C CNN
F 1 "10k" H 5200 4100 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 5280 4050 50  0001 C CNN
F 3 "http://www.yageo.com/documents/recent/PYu-RC_Group_51_RoHS_L_9.pdf" H 5350 4050 50  0001 C CNN
F 4 "RES SMD 10K OHM 1% 1/8W 0805" V 5350 4050 50  0001 C CNN "Description"
F 5 "Yageo" H 5350 4050 50  0001 C CNN "Manufacturer"
F 6 "RC0805FR-0710KL" H 5350 4050 50  0001 C CNN "Part Number"
F 7 "311-10.0KCRCT-ND" H 5350 4050 50  0001 C CNN "Order Number"
F 8 "Digikey" H 5350 4050 50  0001 C CNN "Supplier"
	1    5350 4050
	1    0    0    1   
$EndComp
Wire Wire Line
	5350 3600 4700 3600
Connection ~ 5350 3600
Wire Wire Line
	5550 3700 4700 3700
Connection ~ 5550 3700
Wire Notes Line
	1100 2900 1100 4650
Wire Notes Line
	1100 4650 3600 4650
Wire Notes Line
	3600 4650 3600 2900
Wire Notes Line
	3600 2900 1100 2900
Wire Notes Line
	4500 1950 4500 5050
Wire Notes Line
	4500 5050 9600 5050
Wire Notes Line
	9600 5050 9600 1950
Wire Notes Line
	9600 1950 4500 1950
Text Notes 1900 4600 0    50   ~ 0
To motherboard
Text Notes 5550 4950 0    50   ~ 0
DMX Output (RS485)
$Comp
L Device:R R4
U 1 1 5BBED788
P 7300 3800
F 0 "R4" V 7200 3750 50  0000 C CNN
F 1 "10R" V 7200 3950 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 7230 3800 50  0001 C CNN
F 3 "https://www.rohm.com/datasheet/ESR01MZPF/esr-e" H 7300 3800 50  0001 C CNN
F 4 "RES SMD 10 OHM 5% 0.4W 0805" V 7300 3800 50  0001 C CNN "Description"
F 5 "Digikey" V 7300 3800 50  0001 C CNN "Supplier"
F 6 "ESR10EZPJ100" V 7300 3800 50  0001 C CNN "Part Number"
F 7 "RHM10KCT-ND" V 7300 3800 50  0001 C CNN "Order Number"
F 8 "Rohm" V 7300 3800 50  0001 C CNN "Manufacturer"
	1    7300 3800
	0    1    1    0   
$EndComp
$Comp
L Device:C C1
U 1 1 5BBED9D4
P 7050 2450
F 0 "C1" H 6800 2500 50  0000 L CNN
F 1 "10uF 10V" H 6550 2400 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 7088 2300 50  0001 C CNN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Samsung%20PDFs/CL_Series_MLCC_ds.pdf" H 7050 2450 50  0001 C CNN
F 4 "CAP CER 10UF 10V X5R 0805" H 7050 2450 50  0001 C CNN "Description"
F 5 "CL21A106KPCLQNC" H 7050 2450 50  0001 C CNN "Part Number"
F 6 "1276-2402-1-ND" H 7050 2450 50  0001 C CNN "Order Number"
F 7 "Digikey" H 7050 2450 50  0001 C CNN "Supplier"
F 8 "Samsung Electro-mechanics" H 7050 2450 50  0001 C CNN "Manufacturer"
	1    7050 2450
	1    0    0    -1  
$EndComp
$Comp
L Device:D_TVS_ALT D2
U 1 1 5BD26721
P 8000 4400
F 0 "D2" V 7950 4500 50  0000 L CNN
F 1 "43V 600W" V 8050 4500 50  0000 L CNN
F 2 "project_parts:D_SMB_NP" H 8000 4400 50  0001 C CNN
F 3 "http://www.littelfuse.com/~/media/electronics/datasheets/tvs_diodes/littelfuse_tvs_diode_smbj_datasheet.pdf.pdf" H 8000 4400 50  0001 C CNN
F 4 "TVS DIODE 43V 69.4V DO214AA" V 8000 4400 50  0001 C CNN "Description"
F 5 "SMBJ43CA" V 8000 4400 50  0001 C CNN "Part Number"
F 6 "Digikey" V 8000 4400 50  0001 C CNN "Supplier"
F 7 "Littelfuse" V 8000 4400 50  0001 C CNN "Manufacturer"
F 8 "SMBJ43CALFCT-ND" V 8000 4400 50  0001 C CNN "Order Number"
	1    8000 4400
	0    1    1    0   
$EndComp
$Comp
L faraday:module_connector_header J2
U 1 1 5BCD483D
P 2200 3800
F 0 "J2" H 1850 4350 50  0000 C CNN
F 1 "module_connector_header" H 2250 3250 50  0000 C CNN
F 2 "project_parts:Faraday_module" H 2300 3100 50  0001 C CNN
F 3 "https://cdn.amphenol-icc.com/media/wysiwyg/files/drawing/68000.pdf" H 2300 3100 50  0001 C CNN
F 4 "CONN HEADER MALE 18POS .1\"" H 1950 4450 50  0001 C CNN "Description"
F 5 "Amphenol FCI" H 2200 3800 50  0001 C CNN "Manufacturer"
F 6 "68001-418HLF" H 2200 3800 50  0001 C CNN "Part Number"
F 7 "609-5513-ND" H 2200 3800 50  0001 C CNN "Order Number"
F 8 "Digikey" H 2200 3800 50  0001 C CNN "Supplier"
	1    2200 3800
	1    0    0    -1  
$EndComp
$EndSCHEMATC
