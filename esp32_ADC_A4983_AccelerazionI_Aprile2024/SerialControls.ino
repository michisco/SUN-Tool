void parseCommand(char* buf)
{
    #ifdef LOG_DEBUG
    Serial.printf("received: %s \n", buf);
    #endif
    float ref[8];
    int i_ref[8];
    byte byteval[8];
    char cmd = buf[0];
    
    switch (cmd)
    {  
      case 'a': //SEND STEP COMMAND WITH ACCELERATION
                //es: a 200 500.0 500.0
        sscanf(buf,"%c %d %f %f", &cmd, &i_ref[0], &ref[0], &ref[1]); //target step, max speed, max acceleration
        Serial.println("command Step");

        pos_ref = i_ref[0];
        maxSpeed = ref[0];
        acceleration = ref[1];

        stepperX.setMaxSpeed(maxSpeed);
        stepperX.setAcceleration(acceleration);
        //stepperX.setCurrentPosition(0);
        stepperX.moveTo(pos_ref);
        //Serial.printf("Command received: %c, refs: %f %f %f %f %f\n", cmd, ref[0], ref[1], ref[2], ref[3], ref[4]); 
      break;

      case 'w': //SET MPR121 REGISTER
        sscanf((char*) &buf[0],"%c,%d,%d",&cmd, &byteval[0], &byteval[1]);
        Serial.printf("Command %c: MPR121 set register %d to %d\n", buf[0], byteval[0], byteval[1]); 
        set_register(0x5A, ELE_CFG, 0x00); 
        delay(100);
        set_register(MPR121_ADDR, byteval[0], byteval[1]);
        delay(100);
        set_register(0x5A, ELE_CFG, 0x0C);
      break; 
            
      case 's': //SEND STEP COMMAND
                //es: s 10.0 5.0
        sscanf(buf,"%c %f %f", &cmd, &ref[0], &ref[1]);
        //Serial.printf("Command step: %c, steps: %f velocitity (steps/second) %f\n", cmd, ref[0], ref[1]);

        step_ref = ref[0]; //16 microsteps

        if(ref[1]>0)
        {
          steps_cnt = steps_cnt+ref[0];
          digitalWrite(DIR_PIN, HIGH);
        }
        else
        {  
          steps_cnt = steps_cnt-ref[0];
          digitalWrite(DIR_PIN, LOW);
        }
        if(ref[1]!=0)
          step_time_us = (int)(1000000.0/abs(ref[1]))-STEP_PULSE_TIME_US;

        //digitalWrite(EN_STEP_PIN, HIGH);
        for(int i=0; i<step_ref; i++)
        {
          digitalWrite(STEP_PIN, HIGH);
          delayMicroseconds(STEP_PULSE_TIME_US);
          digitalWrite(STEP_PIN, LOW);
          delayMicroseconds(step_time_us);
        }
        //digitalWrite(EN_STEP_PIN, LOW);     
      break;

      
      default:
        Serial.printf("Comando non riconosciuto\n");
      break; 
    }
}


void SerialRead(void)
{

  int i = 0;
  int a = 0;
  char cmd = ' ';
  char buf_rx[64];
  
  a = Serial.available();
  while (a>0)
  {
    buf_rx[i] = Serial.read();
    i++;
    a = Serial.available();
    if(a<=0)
    {
      delayMicroseconds(500);
      a = Serial.available();
    }
  }
  buf_rx[i] = '\0';
  if (i > 0)
  {
    parseCommand(buf_rx);
  }
  buf_rx[0] = 0;
}
