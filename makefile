all:	bme280sensor windsensor rainsensor wu utils weatherdata weather

sqlite3: sqlite3.c sqlite3.h
	gcc -c sqlite3.c

sqlitedb: sqlitedb.cpp sqlitedb.h
	g++ -c sqlitedb.cpp

bme280sensor: bme280sensor.cpp bme280sensor.h
	g++ -c bme280sensor.cpp

windsensor: windsensor.cpp windsensor.h
	g++ -c windsensor.cpp

rainsensor: rainsensor.cpp rainsensor.h
	g++ -c rainsensor.cpp

wu: wu.cpp wu.h
	g++ -c wu.cpp

utils: utils.cpp utils.h
	g++ -c utils.cpp

weatherdata: weatherdata.cpp weatherdata.h
	g++ -c weatherdata.cpp

weather: bme280sensor windsensor rainsensor sqlitedb utils weatherdata wu main.cpp
	g++ -o weather main.cpp bme280sensor.o windsensor.o rainsensor.o wu.o utils.o weatherdata.o sqlite3.o sqlitedb.o -lwiringPi -lpthread -ldl
	strip weather

clean:
	rm -f weather bme280sensor.o windsensor.o rainsensor.o utils.o wu.o weatherdata.o sqlitedb.o
