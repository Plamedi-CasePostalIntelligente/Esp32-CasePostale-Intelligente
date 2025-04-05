#ifndef MyButton_H_
#define MyButton_H_

class MyButton
{
private:
    int _buttonPin;
public:
    MyButton(int buttonPin);
    ~MyButton();
    
    int ButtonPressed();
};

#endif