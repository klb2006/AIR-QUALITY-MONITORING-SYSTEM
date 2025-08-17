//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// EXAMPLE for SDS011_vers.cpp       dust sensor PM2.5 and PM10  ist auch examples.ino
// -------------------------------------------------------------------------------------
//
// Basied on SDS011 By R. Zschiegner (rz@madavi.de)
//
// & Documentation: The iNovaFitness Laser Dust Sensor Control Protocol V1.4   SDS011
//
//--------------------------------------------------------------------------------------

//uses any documented function:

// 1 -
// 2 Set Data Reporting Mode to ActiveReporting(DEFAULT), to QueryReporting
// 3 -
// 4 Read PM QueryDataCommand, Read
//   read_q for query mode
//   read_a for active mode
// 5 Set DeviceID 0xFF = just read
// 6 Set Sleep set Work(DEFAULT)
// 7 CheckFirmwareVersion
// 8 Set Working Period 1-30 min, set Continuous (DEFAULT) = 0 min

//--------------------------------------------------------------------------------------



#include <SDS011_vers.h>

SDS011_vers my_sdsv;

int error = 1 ;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);     // für Ausgabe im seriellen Monitor
  my_sdsv.begin(D1, D2);

  //------------------------------------------


  // geht
  error = my_sdsv.SetWork();
  Serial.print("ino SetWork Error: "); Serial.println(error);
  //  2 ------------------
  //--------------------------------------------------------------------------------------
  //  int SetDataReportingMode(queryset,activequery,DevID1c,DevID2c,querysetr,acticequeryr,DevID1r,DevID2r)
  //      still effective after power off
  //--------------------------------------------------------------------------------------
  int queryset = 1;    // 1 set Mode
  int activequery = 1;  // 0 activereport Mode (DEFAULT)  1 querymode
  int DevID1c = 0xFF;  // FF= all
  int DevID2c = 0xFF;  // FF= all

  int querysetr ;
  int activequeryr;
  int DevID1r;
  int DevID2r;


  error = my_sdsv.SetDataReportingMode(&queryset, &activequery, &DevID1c, &DevID2c, &querysetr, &activequeryr, &DevID1r, &DevID2r);



  Serial.print("SetDataReportingMode:1 "); Serial.print(querysetr); Serial.print("  -  "); Serial.println(activequeryr);
  Serial.print("DeviceID               "); Serial.print(DevID1r, HEX); Serial.print(" - "); Serial.println(DevID2r, HEX);
  Serial.print("SetDataReportingError: "); Serial.println(error);
  //----------------------------------------
  delay(3000);

  /**/


  /* geht
     //   nochmal lesen
    queryset = 0;    // 1 set Mode
    activequery = 0;  // 0 activereport Mode (DEFAULT)  1 querymode
    DevID1c = 0xFF;  // FF= all
    DevID2c = 0xFF;  // FF= all

    querysetr = 3;
    activequeryr = 3;
    DevID1r = 3;
    DevID2r = 3;

    error = my_sdsv.SetDataReportingMode(&queryset, &activequery, &DevID1c, &DevID2c, &querysetr, &activequeryr, &DevID1r, &DevID2r);



    Serial.print("SetDataReportingMode:2 "); Serial.print(querysetr); Serial.print("  -  "); Serial.println(activequeryr);
    Serial.print("DeviceID               "); Serial.print(DevID1r, HEX); Serial.print(" - "); Serial.println(DevID2r, HEX);
    Serial.print("SetDataReportingError: "); Serial.println(error);


    //delay(3000);

    /**/


  //  geht


  //my_sdsv.wakeup();


  //  8 ------------------
  //--------------------------------------------------------------------------------------
  //  ( effective after power off)  ( continuous mode factory default)
  //
  // int SetWorkingPeriod(queryset,contperiod,DevID1c,DevID2c,querysetr,contperiodr,DevID1r,DevID2r)
  //--------------------------------------------------------------------------------------
  queryset = 1;    // 1 set Mode
  int contperiod = 0; // 0 Continuous Mode (DEFAULT), Working Period 1-30 min (incl 30 sec Measurement)
  DevID1c = 0xFF;  // FF= all
  DevID2c = 0xFF;  // FF= all

  querysetr = 3;
  int contperiodr = 3;
  DevID1r = 3;
  DevID2r = 3;


  Serial.print(" SetWorkingPeriodv1:       "); Serial.print(queryset); Serial.print("_"); Serial.print(contperiod); Serial.print("_"); Serial.print(DevID1c, HEX); Serial.print("_"); Serial.println(DevID2c, HEX);
  Serial.print(" SetWorkingPeriodv1 Error: "); Serial.println(error);

  error = my_sdsv.SetWorkingPeriod(&queryset, &contperiod, &DevID1c, &DevID2c, &querysetr, &contperiodr, &DevID1r, &DevID2r);


  Serial.print(" SetWorkingPeriodn1      : "); Serial.print(querysetr); Serial.print("_"); Serial.print(contperiodr); Serial.print("_"); Serial.print(DevID1r, HEX); Serial.print("_"); Serial.println(DevID2r, HEX);
  Serial.print(" SetWorkingPeriodn1 Error: "); Serial.println(error);


  // nochmal lesen

  queryset = 0;    // 1 set Mode
  contperiod = 0; // 0 Continuous Mode (DEFAULT), Working Period 1-30 min (incl 30 sec Measurement)
  DevID1c = 0xFF;  // FF= all
  DevID2c = 0xFF;  // FF= all

  querysetr = 3;
  contperiodr = 3;
  DevID1r = 3;
  DevID2r = 3;


  Serial.print(" SetWorkingPeriodv2:       "); Serial.print(queryset); Serial.print("_"); Serial.print(contperiod); Serial.print("_"); Serial.print(DevID1c, HEX); Serial.print("_"); Serial.println(DevID2c, HEX);
  Serial.print(" SetWorkingPeriodv2 Error: "); Serial.println(error);

  error = my_sdsv.SetWorkingPeriod(&queryset, &contperiod, &DevID1c, &DevID2c, &querysetr, &contperiodr, &DevID1r, &DevID2r);


  Serial.print(" SetWorkingPeriodn2:       "); Serial.print(querysetr); Serial.print("_"); Serial.print(contperiodr); Serial.print("_"); Serial.print(DevID1r, HEX); Serial.print("_"); Serial.println(DevID2r, HEX);
  Serial.print(" SetWorkingPeriodn2 Error: "); Serial.println(error);






  //----------------------------------------



  delay(3000);
  /**/

  /*
    // geht

    my_sdsv.wakeup();

      //  7 ------------------
      //--------------------------------------------------------------------------------
      //  check firmware version  FirmwareVersion: Year 20-17 Month 8 Day 21
      //  DeviceID: 0x92-0x7A
      //--------------------------------------------------------------------------------
      int Y_ear = 0;
      int M_onth = 0;
      int D_ay = 0;
      int D_evB1 = 0;
      int D_evB2 = 0;

      error = my_sdsv.CheckFirmwareVersion(&Y_ear, &M_onth, &D_ay, &D_evB1, &D_evB2);
      if (error == 0) {
        Serial.println();
        Serial.print("FirmwareVersion: "); Serial.print("Year 20-"); Serial.print(Y_ear); Serial.print(" Month "); Serial.print(M_onth); Serial.print(" Day "); Serial.println(D_ay);
        Serial.print("       DeviceID: "); Serial.print("0x"); Serial.print(D_evB1, HEX); Serial.print("-0x");  Serial.println(D_evB2, HEX);
      }   else {
        Serial.print("CheckFirmwareVersion Error sketch: ");
        Serial.println(error);
      }

      /*  */
  //----------------------------------------
  /*

    error =  my_sdsv.CheckFirmwareVersion2(&Y_ear, &M_onth, &D_ay, &D_evB1, &D_evB2);
    if (error == 0) {
      Serial.println();
      Serial.print("FirmwareVersion2: "); Serial.print("Year 20-"); Serial.print(Y_ear); Serial.print(" Month "); Serial.print(M_onth); Serial.print(" Day "); Serial.print(D_ay);
      Serial.print("        DeviceID: "); Serial.print("0x"); Serial.print(D_evB1, HEX); Serial.print("-0x");  Serial.println(D_evB2, HEX);
    }   else {
      Serial.print("CheckFirmwareVersion2Error: ");
      Serial.println(error);
    }

  */



  /*
    // geht

    //  5 ----------------------------------------
    //--------------------------------------------------------------------------------------
    //  SetDeviceID   DeviceID: 0x92-0x7A
    //--------------------------------------------------------------------------------------
    int DevID1c = 0x92;  //newID
    int DevID2c = 0x7A;  //newID
    int DevID1r = 0xFF;  //oldID  FF = read
    int DevID2r = 0xFF;  //oldID

    error = my_sdsv.SetDeviceID(&DevID1c, &DevID2c, &DevID1r, &DevID2r);
    Serial.print("ino SetDeviceID: "); Serial.print(DevID1r, HEX); Serial.println(DevID2r, HEX);
    Serial.print("ino SetDeviceID Error: "); Serial.println(error);
    //----------------------------------------

    /*  */





  //-----------------------------------------
  //my_sdsv.sleep();


  /*
    error = my_sdsv.SetSleep();
    Serial.print("ino SetSleep Error: "); Serial.println(error);
    /**/
  Serial.println("END SETUP");



}


//----------------------------------------------





void loop() {

  // geht
  //  6 ----------------------------------------
  // --------------------------------------------------------
  // SDS011_vers:SetSleepAndWork
  // --------------------------------------------------------
  int   queryset = 1; // 1 set Mode
  int sleepwork = 1; // 0 sleep
  int  DevID1c = 0xFF;
  int  DevID2c = 0xFF;
  int  querysetr = 0;
  int sleepworkr = 0;
  int  DevID1r = 0;
  int  DevID2r = 0;

  error = my_sdsv.SetSleepAndWork(&queryset, &sleepwork, &DevID1c, &DevID2c, &querysetr, &sleepworkr, &DevID1r, &DevID2r);

  Serial.print("SetSleepAndWork1:      "); Serial.print(querysetr); Serial.print("  -  "); Serial.println(sleepworkr);
  Serial.print("DeviceID               "); Serial.print(DevID1r, HEX); Serial.print(" - "); Serial.println(DevID2r, HEX);
  Serial.print("SetSleepAndWork Error: "); Serial.println(error);
  //----------------------------------------
  delay(500);
  /**/


  /*
    // geht aber unklar wg setsleep
    error = my_sdsv.SetWork();
    Serial.print("ino SetWork Error: "); Serial.println(error);
    /**/












  //   geht
  //  4 ----------------------------------------
  // -------------------------------------------------------------------------------------
  // SDS011_vers::QueryDataCommand
  // -------------------------------------------------------------------------------------
  float p25 = 0;
  float p10 = 0;
  DevID1c = 0xFF;
  DevID2c = 0xFF;
  DevID1r = 0;
  DevID2r = 0;

  error = my_sdsv.QueryDataCommand(&DevID1c, &DevID2c, &p25, &p10, &DevID1r, &DevID2r);

  Serial.println("QueryDataCommand:      ");
  Serial.print  ("PM2.5 :                "); Serial.print(p25, 2); Serial.print("  PM10 : "); Serial.println(p10, 2);
  Serial.print  ("DeviceID :             "); Serial.print(DevID1r, HEX); Serial.print(" - "); Serial.println(DevID2r, HEX);
  Serial.print  ("QueryDataCommand Error:"); Serial.println(error);
  //  ----------------------------------------

  // delay(3500);
  /**/

  /*

    // --------------------------------------------------------
    // SDS011_vers:Read
    // --------------------------------------------------------
    //  float p25 = 0;
    //  float p10 = 0;
    int  error = my_sdsv.Read(&p25, &p10);
    Serial.print("Read: "); Serial.print(p25); Serial.print("  "); Serial.println(String(p10));
    Serial.print("Read Error: "); Serial.println(error);
    //----------------------------------------
    // delay(3000);
    /* */


  //  geht
  //  6 ----------------------------------------
  // --------------------------------------------------------
  // SDS011_vers:SetSleepAndWork
  // --------------------------------------------------------
  queryset = 1; // 1 set Mode
  sleepwork = 0; // 0 sleep
  DevID1c = 0xFF;
  DevID2c = 0xFF;
  querysetr = 0;
  sleepworkr = 0;
  DevID1r = 0;
  DevID2r = 0;

  error = my_sdsv.SetSleepAndWork(&queryset, &sleepwork, &DevID1c, &DevID2c, &querysetr, &sleepworkr, &DevID1r, &DevID2r);

  Serial.print("SetSleepAndWork2:      "); Serial.print(querysetr); Serial.print("  -  "); Serial.println(sleepworkr);
  Serial.print("DeviceID               "); Serial.print(DevID1r, HEX); Serial.print(" - "); Serial.println(DevID2r, HEX);
  Serial.print("SetSleepAndWork Error: "); Serial.println(error);
  //----------------------------------------
  /**/


  /*  macht probleme adresse von adresse?
    error = my_sdsv.SetSleep();
    Serial.print("ino SetSleep Error: "); Serial.println(error);

    /**/


  delay(25000);  // 55000+,5+,5+4 =60
}
// end loop


