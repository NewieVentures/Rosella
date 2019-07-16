EESchema Schematic File Version 4
LIBS:FA1-cache
EELAYER 29 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Faraday FA1"
Date "2018-10-18"
Rev "A"
Comp "Newie Ventures"
Comment1 "M Bright"
Comment2 "UTS"
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L faraday:FP1 U1
U 1 1 5BB73006
P 5650 3850
F 0 "U1" H 5200 4450 50  0000 C CNN
F 1 "FP1 sub-assembly" H 5500 3250 50  0000 C CNN
F 2 "" H 6150 3750 50  0001 C CNN
F 3 "" H 6150 3750 50  0001 C CNN
F 4 "FP1 sub-assembly" H 5650 3850 50  0001 C CNN "Description"
F 5 "NewieVentures" H 5650 3850 50  0001 C CNN "Manufacturer"
F 6 "NewieVentures" H 5650 3850 50  0001 C CNN "Supplier"
F 7 "FP1" H 5650 3850 50  0001 C CNN "Order Number"
F 8 "FP1" H 5650 3850 50  0001 C CNN "Part Number"
	1    5650 3850
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J1
U 1 1 5BB73310
P 2450 3500
F 0 "J1" H 2450 3700 50  0000 C CNN
F 1 "240V mains" H 2450 3300 50  0000 C CNN
F 2 "" H 2450 3500 50  0001 C CNN
F 3 "http://www.qualtekusa.com/Catalog/AC_Receptacles/pdfs/703w0004.pdf" H 2450 3500 50  0001 C CNN
F 4 "Qualtek" H 2450 3500 50  0001 C CNN "Manufacturer"
F 5 "703W-00/04" H 2450 3500 50  0001 C CNN "Part Number"
F 6 "Digikey" H 2450 3500 50  0001 C CNN "Supplier"
F 7 "Q212-ND" H 2450 3500 50  0001 C CNN "Order Number"
F 8 "PWR ENT RCPT IEC320-C14 PANEL QC" H 2450 3500 50  0001 C CNN "Description"
	1    2450 3500
	-1   0    0    -1  
$EndComp
$Comp
L power:Earth #PWR0101
U 1 1 5BB73504
P 2800 3750
F 0 "#PWR0101" H 2800 3500 50  0001 C CNN
F 1 "Earth" H 2800 3600 50  0001 C CNN
F 2 "" H 2800 3750 50  0001 C CNN
F 3 "~" H 2800 3750 50  0001 C CNN
	1    2800 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	2800 3750 2800 3600
Wire Wire Line
	2800 3600 2650 3600
$Comp
L Connector:Conn_Coaxial J2
U 1 1 5BBA8629
P 8500 1950
F 0 "J2" H 8450 2200 50  0000 L CNN
F 1 "Cellular Antenna" H 8250 2100 50  0000 L CNN
F 2 "" H 8500 1950 50  0001 C CNN
F 3 "https://www.amphenolrf.com/media/downloads/2077/132105.pdf" H 8500 1950 50  0001 C CNN
F 4 "132105" H 8500 1950 50  0001 C CNN "Part Number"
F 5 "Amphenol" H 8500 1950 50  0001 C CNN "Manufacturer"
F 6 "Digikey" H 8500 1950 50  0001 C CNN "Supplier"
F 7 "ACX1183-ND" H 8500 1950 50  0001 C CNN "Order Number"
F 8 "CONN SMA JACK STR 50 OHM SOLDER" H 8500 1950 50  0001 C CNN "Description"
	1    8500 1950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 5BBA878F
P 8500 2300
F 0 "#PWR0102" H 8500 2050 50  0001 C CNN
F 1 "GND" H 8505 2127 50  0000 C CNN
F 2 "" H 8500 2300 50  0001 C CNN
F 3 "" H 8500 2300 50  0001 C CNN
	1    8500 2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	8500 2300 8500 2150
$Comp
L power:Earth #PWR0103
U 1 1 5BBA890F
P 5000 3650
F 0 "#PWR0103" H 5000 3400 50  0001 C CNN
F 1 "Earth" H 5000 3500 50  0001 C CNN
F 2 "" H 5000 3650 50  0001 C CNN
F 3 "~" H 5000 3650 50  0001 C CNN
	1    5000 3650
	1    0    0    -1  
$EndComp
Wire Wire Line
	5000 3600 5050 3600
$Comp
L Connector_Generic:Conn_01x04 J3
U 1 1 5BBA8CAC
P 8500 4200
F 0 "J3" H 8450 4400 50  0000 L CNN
F 1 "DMX Out" H 8350 3900 50  0000 L CNN
F 2 "" H 8500 4200 50  0001 C CNN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Phoenix%20Contact%20PDFs/1899155.pdf" H 8500 4200 50  0001 C CNN
F 4 "1899155" H 8500 4200 50  0001 C CNN "Part Number"
F 5 "Phoenix Contact" H 8500 4200 50  0001 C CNN "Manufacturer"
F 6 "Digikey" H 8500 4200 50  0001 C CNN "Supplier"
F 7 "277-6689-ND" H 8500 4200 50  0001 C CNN "Order Number"
F 8 "TERM BLK HEADER 4POS GREEN" H 8500 4200 50  0001 C CNN "Description"
	1    8500 4200
	1    0    0    -1  
$EndComp
$Comp
L faraday:PS-05 U2
U 1 1 5BBAA826
P 4100 4250
F 0 "U2" H 4100 4617 50  0000 C CNN
F 1 "PS-05-5" H 4100 4526 50  0000 C CNN
F 2 "" H 8550 6500 50  0001 C CNN
F 3 "https://www.meanwell.com/Upload/PDF/PS-05/PS-05-SPEC.PDF" H 8550 6500 50  0001 C CNN
F 4 "PS-05-5" H 4100 4250 50  0001 C CNN "Part Number"
F 5 "Mean Well" H 4100 4250 50  0001 C CNN "Manufacturer"
F 6 "Digikey" H 4100 4250 50  0001 C CNN "Supplier"
F 7 "1866-5411-ND" H 4100 4250 50  0001 C CNN "Order Number"
F 8 "AC/DC CONVERTER 5V 5W" H 4100 4250 50  0001 C CNN "Description"
	1    4100 4250
	1    0    0    -1  
$EndComp
$Comp
L power:Earth #PWR0104
U 1 1 5BBAA902
P 4100 4700
F 0 "#PWR0104" H 4100 4450 50  0001 C CNN
F 1 "Earth" H 4100 4550 50  0001 C CNN
F 2 "" H 4100 4700 50  0001 C CNN
F 3 "~" H 4100 4700 50  0001 C CNN
	1    4100 4700
	1    0    0    -1  
$EndComp
Wire Wire Line
	4100 4700 4100 4550
Text Notes 3300 4900 0    50   ~ 0
DNF if using PSU on FP1
Wire Wire Line
	5000 3600 5000 3650
Wire Wire Line
	4500 4150 5050 4150
Wire Wire Line
	4500 4350 4850 4350
Wire Wire Line
	4850 4350 4850 4300
Wire Wire Line
	4850 4300 5050 4300
Wire Wire Line
	3700 4150 3600 4150
Wire Wire Line
	3700 4350 3400 4350
Wire Notes Line
	3250 4950 4700 4950
Wire Notes Line
	4700 4950 4700 3800
Wire Notes Line
	4700 3800 3250 3800
Wire Notes Line
	3250 3800 3250 4950
Wire Wire Line
	8300 4400 8200 4400
Wire Wire Line
	8200 4400 8200 4300
Connection ~ 8200 4300
Wire Wire Line
	8200 4300 8300 4300
Text Notes 8600 4150 0    50   ~ 0
A
Text Notes 8600 4250 0    50   ~ 0
B
Text Notes 8600 4350 0    50   ~ 0
G
Text Notes 8600 4450 0    50   ~ 0
G
Wire Wire Line
	2650 3400 3600 3400
Wire Wire Line
	2650 3500 3400 3500
Connection ~ 3600 3400
Wire Wire Line
	3600 3400 5050 3400
Connection ~ 3400 3500
Wire Wire Line
	3600 3400 3600 4150
Wire Wire Line
	3400 3500 3400 4350
Text Notes 8150 4700 0    50   ~ 0
4-way to match\nexisting controller
$Comp
L Device:LED D1
U 1 1 5BD11634
P 8200 2950
F 0 "D1" V 8238 2832 50  0000 R CNN
F 1 "Power (red)" V 8147 2832 50  0000 R CNN
F 2 "" H 8200 2950 50  0001 C CNN
F 3 "~" H 8200 2950 50  0001 C CNN
F 4 "EG5379-ND" V 8200 2950 50  0001 C CNN "Order Number"
F 5 "PVL16FLS11" V 8200 2950 50  0001 C CNN "Part Number"
F 6 "LED PANEL INDICATOR RED 1.8V IP67" V 8200 2950 50  0001 C CNN "Description"
F 7 "Digikey" V 8200 2950 50  0001 C CNN "Supplier"
F 8 "E-Switch" V 8200 2950 50  0001 C CNN "Manufacturer"
	1    8200 2950
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED D2
U 1 1 5BD11726
P 8200 3500
F 0 "D2" V 8238 3383 50  0000 R CNN
F 1 "Status (blue)" V 8147 3383 50  0000 R CNN
F 2 "" H 8200 3500 50  0001 C CNN
F 3 "~" H 8200 3500 50  0001 C CNN
F 4 "EG5381-ND" V 8200 3500 50  0001 C CNN "Order Number"
F 5 "PVL16FLS41" V 8200 3500 50  0001 C CNN "Part Number"
F 6 "LED PANEL INDICATOR BLUE 2.8V IP67" V 8200 3500 50  0001 C CNN "Description"
F 7 "Digikey" V 8200 3500 50  0001 C CNN "Supplier"
F 8 "E-Switch" V 8200 3500 50  0001 C CNN "Manufacturer"
	1    8200 3500
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6250 3400 7000 3400
Wire Wire Line
	6250 4100 8300 4100
Wire Wire Line
	6250 4200 8300 4200
Wire Wire Line
	6250 4300 8200 4300
Wire Wire Line
	6250 3900 7500 3900
Wire Wire Line
	7500 3900 7500 3650
Wire Wire Line
	6250 3800 7400 3800
Wire Wire Line
	7400 3800 7400 3350
Wire Wire Line
	6250 3650 7250 3650
Wire Wire Line
	7250 3650 7250 3100
Wire Wire Line
	6250 3550 7150 3550
Wire Wire Line
	7150 3550 7150 2800
Wire Wire Line
	7150 2800 8200 2800
Wire Wire Line
	3400 3500 5050 3500
Wire Notes Line
	7950 2700 9000 2700
Wire Notes Line
	9000 2700 9000 3800
Wire Notes Line
	9000 3800 7950 3800
Wire Notes Line
	7950 3800 7950 2700
Text Notes 8350 3750 0    50   ~ 0
Indicators
Wire Notes Line
	7950 3900 7950 4750
Wire Notes Line
	7950 4750 9000 4750
Wire Notes Line
	9000 4750 9000 3900
Wire Notes Line
	9000 3900 7950 3900
Wire Wire Line
	7000 1950 8300 1950
Wire Notes Line
	7950 2650 9000 2650
Wire Notes Line
	9000 2650 9000 1600
Wire Notes Line
	9000 1600 7950 1600
Wire Notes Line
	7950 1600 7950 2650
Text Notes 8200 2600 0    50   ~ 0
External Antenna
Wire Wire Line
	7000 1950 7000 3400
Wire Wire Line
	7250 3100 8200 3100
Wire Wire Line
	7400 3350 8200 3350
Wire Wire Line
	7500 3650 8200 3650
$EndSCHEMATC
