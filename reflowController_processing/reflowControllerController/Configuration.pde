public interface Configuration {
  int getBaudRate();
  int getPortNumber();
  long getSaveIntervalMils();
  String getDataOutputDirectory();
}

public class DefaultConfiguration implements Configuration {
  private static final int DEFAULT_BAUDRATE = 4800;
  private static final int DEFAULT_PORTNUMBER = 5;
  private static final long DEFAULT_SAVE_INTERVAL_MILLIS = 100l;
  private static final String DEFAULT_OUTPUT_DIR = "zz_outputData";
  
  public int getBaudRate() {
    return DEFAULT_BAUDRATE;  
  }
  
  public int getPortNumber() {
    return DEFAULT_PORTNUMBER;
  }
  
  public long getSaveIntervalMils() {
    return DEFAULT_SAVE_INTERVAL_MILLIS;
  }
  
  public String getDataOutputDirectory() {
    return DEFAULT_OUTPUT_DIR;
  }
}