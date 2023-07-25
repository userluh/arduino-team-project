#include<Servo.h>//서보 함수를 사용하기위해 사용함
const int buttonPins[5] = {8,9,10,11,12}; // 버튼 핀맵
const int SERVO = 13;//서보모터의 핀번호 지정
Servo servo;

int buttonPressCount[4] = {0, 0, 0, 0};// 각 버튼의 누른 횟수를 저장하는 배열
int a, b, c, d;// 계산된 값들을 저장할 변수

const int led[3]={4,5,6};//led의 핀번호 지정

const int BUZZER=7;//부저의 핀번호지정 
const int trig_pin=3;//초음파센서의 핀지정
const int echo_pin=2;//초음파센서의 핀지정


void setup() {

  pinMode(trig_pin,OUTPUT);//trig_pin은 출력으로 지정
  pinMode(echo_pin,INPUT);//echo_pin은 입력으로 지정
  servo.attach(SERVO);
  servo.write(0);//서보모터의 위치를 초기화 시킴


  for (int i = 0; i < 5; i++) { // 버튼을 입력으로 설정
    pinMode(buttonPins[i], INPUT); 
  }
  for (int i = 0; i < 3; i++) {//led를 출력으로 설정
    pinMode(led[i], OUTPUT); 
  }
  pinMode(BUZZER, OUTPUT);//부저를 출력으로 설정
  Serial.begin(115200);//시리얼 모니터의 속도를 115200으로 설정

  while(1)//도어락의 기능을 반복시키기위해 while문을 걸고 특정한 조건에서 while문을 빠져나와 프로그램을 종료시킴
  {
   yellow_led();//물체가 감지됐을때 형광등 켜기
 
  for (int i = 0; i < 4; i++) {// 버튼 상태를 확인하고 누른 횟수를 증가시킴

    if (digitalRead(buttonPins[i]) == HIGH) {

      buttonPressCount[i]++;
      delay(1000);  // 버튼 바운스 방지를 위한 딜레이
    }
  }

  // 계산된 값들 갱신
  a = buttonPressCount[0] * 1000; //1000의자리 비밀번호가 눌린횟수*1000
  b = buttonPressCount[1] * 100; //100의 자리 비밀번호가 눌린횟수*100
  c = buttonPressCount[2] * 10;// 10의자리 비밀번호가 눌린횟수*10
  d = buttonPressCount[3];// 1의 자리 비밀번호가 눌린횟수*1

  // 정답인 경우 
 if (digitalRead(buttonPins[4]) == HIGH && (a + b + c + d) == 2134) {//엔터키를 누르고 각자리의 비밀번호를 모두 더한값이 2134(내장된 비밀번호) 이면

//패스워드와 문이 열린다는 문구를 출력함
   Serial.print("password = ");
   Serial.println(a + b + c + d);
   Serial.println("Door is opened");

   playVictoryMelody();  // 정답을 의미하는 음악 재생,부저 울리기

   digitalWrite(led[0],HIGH);//청색 led 점등

   //1초후에 자동으로 꺼짐
   delay(1000);
   digitalWrite(led[0],LOW);

   servo.write(90);//서보모터 문열림

   //3초후에 자동으로 문닫힘
   delay(3000);
   servo.write(0);


   Serial.println("Door is closed");//문이닫혔다는 문구와 함께 
   break; // while문을 종료함 
 }
  //오답인 경우 
 else if(digitalRead(buttonPins[4]) == HIGH && (a + b + c + d) != 2134)//엔터키를 누르고 각자리의 비밀번호를 모두 더한값이 2134(내장된 비밀번호)가 아니라면 
 {
   Serial.println("Wrong number");//답이틀렸다는 문구를 출력하고

   playErrorMelody();  // 오답을 의미하는 음악 재생
   digitalWrite(led[1],HIGH);//적색led점등

   //1초후 자동으로 꺼짐
   delay(1000);
   digitalWrite(led[1],LOW);

   //입력했던 비밀번호를 초기화하는 과정
   a,b,c,d=0;
   for(int i=0 ; i<=3; i++)
   
   {
     buttonPressCount[i]=0; 
   }

   //오답횟수 카운트
   static int cnt=0; //카운트 변수를 static으로 설정해서 cnt=0 문구를 만나도 0으로 초기화 되지않게끔 만듦, 전역변수를 사용하는것도 가능함
   cnt++;

    //오답횟수가 5회일경우 방범시스템 동작
   if(cnt==5)
   {
     crimesystem();
   }
 }//else if문 닫기
 }//while문 닫기
 }//setup() 닫기
 

//사용하지않음
void loop() {
}



//여기서 부터 사용자 정의 함수
 


// 정답음악 재생함수
void playVictoryMelody() {
  tone(BUZZER, 523, 250);  
  delay(250);
  tone(BUZZER, 659, 250);  
  delay(250);
  tone(BUZZER, 784, 250);  
  delay(250);
  tone(BUZZER, 988, 500);  
  delay(500);
  noTone(BUZZER);  // 음악 재생 종료
}
  
//오답음악 재생함수
void playErrorMelody() {
  tone(BUZZER, 440);  
  delay(250);
  tone(BUZZER, 392);  
  delay(250);
  tone(BUZZER, 349);  
  delay(250);
  tone(BUZZER, 330);  
  delay(500);
  noTone(BUZZER);  // 음악 재생 종료
}

//형광등을 켜는함수
void yellow_led()
{
  //파형을 발생시키고
  digitalWrite(trig_pin,LOW);
  delayMicroseconds(2);
  digitalWrite(trig_pin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin,LOW);

  //에코핀으로 들어오는데 까지의 시간을 계산하여
  long duration=pulseIn(echo_pin,HIGH);

  //거리를 구함
  long distance=(duration*0.034)/2;


   
  if(distance == 10)//만약 감지된 물체까지의 거리가 10cm이면
  {
    digitalWrite(led[2],HIGH);//형광등을 자동으로 킴

    //2초후 자동으로 꺼짐 
    delay(2000);
    digitalWrite(led[2],LOW);
  }
  
}

//방범시스템
void crimesystem() {

  //경찰차 led 점멸, 적색 청색 led가 짧은간격으로 껏다켜지기를 반복하여 점등시킨다
  for(int i=1; i<=10; i++)
  {
    digitalWrite(led[1],HIGH);

    delay(50);
    digitalWrite(led[1],LOW);

    delay(100);
    digitalWrite(led[0],HIGH);
    
    delay(50);
    digitalWrite(led[0],LOW);
  }

  //방범시스템 음악
  int sirenMelody[] = {
    262, 294, 330, 349, 392, 440, 494, 523, 494, 440, 392, 349, 330, 294
  };
  
  for (int i = 0; i < 14; i++) {
    tone(BUZZER, sirenMelody[i]);
    delay(200);
    }
  noTone(BUZZER);
  //10초간 도어락 정지
  delay(10000);
}
