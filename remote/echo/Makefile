
port = /dev/ttyUSB*

all: .crumb

.crumb: echo.ino global.h
	arduino-cli compile -b arduino:avr:uno -v
	arduino-cli upload -p $(port) -b arduino:avr:uno -v
	touch .crumb

clean:
	rm .crumb
