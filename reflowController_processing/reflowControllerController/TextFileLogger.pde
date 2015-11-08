import processing.serial.*;
import java.util.Timer;
import java.util.TimerTask;
import java.io.Writer;

public class TextFileLogger implements Logger {
  
  private final Serial mSerialInput;
  private final Writer mOutputWriter;
  private final long mSaveInterval;
  private static final long TIMER_DELAY_MS = 100l;
  private Timer mSaveTimer;
  private boolean mDirty = false;

  public TextFileLogger(Serial serialInput, Writer outputWriter , long saveInterval) {
    mSerialInput = serialInput;
    mSaveInterval = saveInterval;
    mOutputWriter = outputWriter;
  }
  
  public void logCurrentSerialOutput() {
    if (timerIsUninitialized()) {
      startSaveTimer();
    }
    if (mSerialInput.available() > 0) {       
      String input = mSerialInput.readStringUntil('\n');
      if (!isNullOrWhitespace(input))
      {
        try {                  
          mOutputWriter.write(input);
          print(input);
          mDirty = true;
        } catch (IOException ex) {
          println("EXCEPTION " + ex.getMessage());
        }
      }
    }
  }
  
  private void startSaveTimer() {
    mSaveTimer = new Timer();
    TimerTask timerTask = new TimerTask() {
      @Override
      public void run () {        
        saveLogIfDirty();
      }
    };
    mSaveTimer.scheduleAtFixedRate(timerTask, TIMER_DELAY_MS, mSaveInterval);
  }
  
  private boolean timerIsUninitialized() {
    return mSaveTimer == null;
  }
  
  private void saveLogIfDirty() {    
    try
    {
      if (mDirty) {   
        mOutputWriter.flush();
        mDirty = false;
      }
    } catch (IOException ex) {
      println("EXCEPTION: " + ex.getMessage());      
    }
  }
  
  /**
  TODO: put this in its own utilities class or whatever.
  see: http://stackoverflow.com/questions/8476588/java-equivalent-of-c-sharp-string-isnullorempty-and-string-isnullorwhitespace
  */
  private boolean isNullOrWhitespace(String input) {
    if (input == null) {
      return true;
    }
    
    int length = input.length();
    if (length > 0) {
        for (int i = 0; i < length; i++) {
            if (!Character.isWhitespace(input.charAt(i))) {
                return false;
            }
        }
        return true;
    }
    return false;
    
  } 
}