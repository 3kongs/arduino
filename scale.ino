#include <LiquidCrystal.h>

/*
* 엘레파츠 전자저울 예제 프로그램 - loadcell + HX711
* 사용 라이브러리 : https://github.com/bogde/HX711
* 
* 아두이노 GPIO 연결
* loadcell / HX711 board
* Dout PIN : D2
* SCK PIN  : D3
*/

#include "HX711.h"
#include <LiquidCrystal_I2C.h>
#include <Wire.h> //LCD 관련 해더


// HX711 circuit wiring
#define LOADCELL_DOUT_PIN         2
#define LOADCELL_SCK_PIN          3

#define TARE_BTN_PIN 8

// scale - 10Kg loadcell : 226 / 5kg loadcell : 372
// ADC 모듈에서 측정된 결과값을 (loadcellValue)값 당 1g으로 변환해 줌
float loadcellValue = 678; // 로드셀 캘리브레이션을 위한 

// LCD주소 0x3f 지정 경우에 따라 0x27 또는 0x20 으로 지정되는 경우도 있음 16글자 2
LiquidCrystal_I2C lcd(0x3f, 16, 2);

HX711 scale;
//0점버튼
int tare_btn = HIGH;

void setup() {
  // put your setup code here, to run once:
// LCD 세팅
  lcd.init();
  lcd.backlight();
  lcd.clear();



  pinMode ( tare_btn, INPUT );
//digitalWrite ( tare_btn, HIGH );
  
  Serial.begin(115200);

  // 로드셀 HX711 보드 pin 설정
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  // 부팅 후 잠시 대기 (2초)
  delay(2000);

  // 측정값 1회 읽어오기
  Serial.print("read: \t\t\t");
  Serial.println(scale.read());

  delay(1000);

  // 스케일 설정
  scale.set_scale(loadcellValue);
  
  // 오프셋 설정(10회 측정 후 평균값 적용) - 저울 위에 아무것도 없는 상태를 0g으로 정하는 기준점 설정(저울 위에 아무것도 올려두지 않은 상태여야 합니다.)   
  scale.tare(10);    

  // 설정된 오프셋 및 스케일 값 확인
  Serial.print("Offset value :\t\t");
  Serial.println(scale.get_offset());
  Serial.print("Scale value :\t\t");
  Serial.println(scale.get_scale());

  // (read - offset) 값 확인 (scale 미적용)
  Serial.print("(read - offset) value: \t");  
  Serial.println(scale.get_value());
  delay(2000);

}

void loop() {
  // put your main code here, to run repeatedly:
  SwitchCheck(); // tare 버튼이 눌러지는지 확인하는 함
  float scale_value;
  scale_value = scale.get_units(5);
  // 오프셋 및 스케일이 적용된 측정값 출력 (5회 측정 평균값) 
  if ( abs(scale_value) < 0.5 ) scale_value = 0;
  Serial.print("value :");
  Serial.print(scale_value, 0);    // 5회 측정 평균값, 소수점 아래 2자리 출력
  Serial.println(" g");
  lcd.setCursor(0,0);
  lcd.print(scale_value, 0);
  lcd.print("g ");
  delay(1000);
  //lcd.clear();

  
  
  // 1초 대기
  
  
}

// 스위치 동작 확인&처리 함수
void SwitchCheck(){

  // 스위치 값 읽기
  tare_btn = digitalRead ( TARE_BTN_PIN );

  // 스위치 상태에 따라 각 동작 진행
  if (tare_btn == LOW){ 

    lcd.clear();
    Serial.println("tare!!\n");
    scale.tare(10);
   
  }else{
    // 스위치 입력이 없으면 딜레이 없이 종료
    return;
  }
  
  delay(2000);

  return;
}
