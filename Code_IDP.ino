// declaring ports 
int pulse = 3;
int ocv = 0;  //Analog pin 0  to read battery voltage
int voltage1 = 1;   // Analog pin 1 to read +resistor voltage
int voltage2 = 2;   // Analog pin 1 to read -resistor voltage
int relay = 4;      // relay switch to cut suply
float inisoc;         //initial curr_soc
float curr_soc;            //current curr_soc
float soc = 0;        //
boolean v = true;
boolean c = true;
const float battery_capacity = 2.2;
const int delay_value = 3000;
const float resistance = 100; // we can change it as per requirment
const float no_cell = 12/4;
int counter = 0;
float adc_factor = 0.004702;
void setup() {
  Serial.begin(9600);  
 // TCCR2B = TCCR2B & B11111000 | B00000001; // for PWM frequency of 31372.55 Hz
  pinMode(ocv, INPUT);
  pinMode(voltage1, INPUT);
  pinMode(voltage2, INPUT);
  pinMode(relay, OUTPUT);  
  pinMode(pulse, OUTPUT);
}
void loop() {
 // analogWrite(pulse, 77);
//----------------------------------------------------------------------------
  int vo_dif_dumm  = (analogRead(voltage1) - analogRead(voltage2));
  float vo_dif = vo_dif_dumm*adc_factor;  //adc
//----------------------------------------------------------------------------
  int x = analogRead(ocv);
  float voltage = x*adc_factor*3.0; // multiplied by potential divider factor   adc
//----------------------------------------------------------------------------
  float voltage_per_cell = voltage/no_cell;
  float current = vo_dif/resistance;
//----------------------------------------------------------------------------
  Serial.print("voltage difference  ");
  Serial.println(vo_dif);
  Serial.print("voltage difference per cell  ");
  Serial.println(voltage_per_cell);
  Serial.print("batteryVolatge  ");
  Serial.println(voltage);
  Serial.print("current  ");
  Serial.println(current);
  //Serial.print("curr_soc  ");
  //Serial.println(int(curr_soc));
//----------------------------------------------------------------------------
  if (counter < 1){
    inisoc = table_soc(voltage_per_cell);
    Serial.print("initial curr_soc  ");
    Serial.println(inisoc);
    soc = inisoc;
  }
//----------------------------------------------------------------------------
  curr_soc = coulomb_count(current);
  c = over_curr_det(current);
  if (counter == 2){
    v = over_vol_det(voltage_per_cell);
    counter = 1;
  }
//----------------------------------------------------------------------------
  boolean Switch = c&&v;
  relay_controller(Switch);
  delay(delay_value);
  counter++;
//----------------------------------------------------------------------------
}
//----------------------------------------------------------------------------
// formula for calculating curr_soc
float curr_soc_calculator(float VPC, float a, float b){
  float soc_cal = VPC*a - b;
  return soc_cal;
}
//----------------------------------------------------------------------------
// table for calculating curr_soc
float table_soc(float voltage_per_cell){
  if((3.452>voltage_per_cell)&&(voltage_per_cell>=3.3)){                  
    inisoc = curr_soc_calculator(voltage_per_cell, 26.55, 88.6);
    }
  else if((3.508>voltage_per_cell)&&(voltage_per_cell>=3.452)){
    inisoc = curr_soc_calculator(voltage_per_cell, 125, 431.1);
    }
  else if((3.595>voltage_per_cell)&&(voltage_per_cell>=3.508)){
    inisoc = curr_soc_calculator(voltage_per_cell, 149, 516.1);
    }
  else if((3.676>voltage_per_cell)&&(voltage_per_cell>=3.595)){
    inisoc = curr_soc_calculator(voltage_per_cell, 344, 1225);
    ;}
  else if((3.739>voltage_per_cell)&&(voltage_per_cell>=3.676)){
    inisoc = curr_soc_calculator(voltage_per_cell, 229.5, 800.9);
    }
  else if((3.967>voltage_per_cell)&&(voltage_per_cell>=3.739)){
    inisoc = curr_soc_calculator(voltage_per_cell, 111.9, 359.9);
    }
  else if((4.039>voltage_per_cell)&&(voltage_per_cell>=3.967)){
    inisoc = curr_soc_calculator(voltage_per_cell, 104.8, 332);
    }
  else if((4.09>voltage_per_cell)&&(voltage_per_cell>=4.039)){
    inisoc = curr_soc_calculator(voltage_per_cell, 91.61, 274.7);
    }
  else if (voltage_per_cell<3.3){
    inisoc = 0;
    }
  else{
    inisoc = 100; 
    }
  return inisoc;       // check it 
}
//----------------------------------------------------------------------------
// implementing coulomb counting method
float coulomb_count(float curr){
  soc = soc + (curr*delay_value)*(0.001/battery_capacity);
  return soc;
}
//----------------------------------------------------------------------------
// controls relay
int relay_controller(boolean Switch){
  digitalWrite(relay, Switch);
}
//----------------------------------------------------------------------------
// protection form over current
boolean over_curr_det(int current){
  boolean current_a ;
  if (current > 20)
    current_a = false;
  else 
    current_a = true;
  return current_a;
}
//----------------------------------------------------------------------------
//protection from over voltage and over discharging
boolean over_vol_det(float voltage_per_cell){
  boolean voltage_a;
  relay_controller(0);
  delay(1000);
  float voltage = analogRead(ocv)*adc_factor*3;  //adc
  float voltage_per_cell_0 = voltage/no_cell;
  float check_soc = table_soc(voltage_per_cell);
  Serial.print("paused \n voltage_per_cell_0");
  Serial.println(voltage_per_cell_0);
  Serial.print("soc-ocv graph SOC  ");
  Serial.println(int(check_soc));
  
  if ((3.5 < voltage_per_cell_0) && (voltage_per_cell_0 < 4.1)){
    voltage_a = true;
     return voltage_a;}
  else{
    voltage_a = false;
     return voltage_a;
  }
}
