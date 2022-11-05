program = servo
port = /dev/ttyUSB*

all: .crumb

.crumb: $(program).ino global.h
	arduino-cli compile -b arduino:avr:uno -v
	arduino-cli upload -p $(port) -b arduino:avr:uno -v
	touch .$(program)

clean:
	rm .$(program)
