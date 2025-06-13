bool go = false;
void setup()
{
    pinMode(12, OUTPUT);
    Serial.begin(9600);
    pinMode(5, INPUT);
}

void loop()
{
    if(digitalRead(5)){
        if(go){
            go = false; noTone(12);
        }else {go = true;} 
        delay(500);
    } 

    if(go){
    Serial.println("Beeping");
    for(int i = 300; i < 1000; i++){
        tone(12, i);
        delay(5);
        noTone(12);
        if(digitalRead(5)){
            if(go){
                go = false; noTone(12); delay(500); break;
            }else {go = true;} 
            delay(500);
        } 
    }
    }
}