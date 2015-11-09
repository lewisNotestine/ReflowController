import processing.serial.*;
import java.io.FileWriter;
import java.io.Writer;
import java.io.File;

Serial inputFromMcu;
Configuration config;
Logger logger;
Writer writer;

void setup() {
  config = new DefaultConfiguration();
  inputFromMcu = new Serial( this, Serial.list()[config.getPortNumber()], config.getBaudRate());
  String path = sketchPath(config.getDataOutputDirectory());
  String fileName = "testLog.log";
  File file = new File(path, fileName);
  try {
    writer = new FileWriter(file);
  } catch(IOException ex) {
    println("EXCEPTION: " + ex.getMessage());
    println(String.format("could not open file: %n", file)); 
    exit(); 
  }
  logger = new TextFileLogger(inputFromMcu, writer, config.getSaveIntervalMils());  
  logger.beginLogging();
}

void draw() {
  logger.logCurrentSerialOutput();
}