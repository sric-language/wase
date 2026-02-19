

#include "IosTextInput.h"
#include "IosWindow.h"

extern float g_screenScle;

using namespace waseGraphics;


NSString* fireTextChangeEvent(NSString *text) {
    IosWindow* win = dynamic_cast<IosWindow*>(waseGraphics::Window::getCur().get());
    win->_textInput->onTextChange(text.UTF8String);
    return nil;
}

@interface EditTextListener ()

@end

@implementation EditTextListener

- (instancetype)init {
    self = [super init];
    return self;
}

- ( BOOL )textField:( UITextField  *)textField shouldChangeCharactersInRange:(NSRange )range replacementString:( NSString  *)string {
    return true;
}

- (void)textViewDidChange:(UITextView *)textView {
    NSString* t = fireTextChangeEvent(textView.text);
//    if (t) {
//        textView.text = t;
//    }
}

- (BOOL)textFieldShouldBeginEditing:(UITextField *)textField {
    return true;
}

- (void)textFieldDidBeginEditing:(UITextField *)textField {
    
}

- (BOOL)textFieldShouldEndEditing:(UITextField *)textField {
    return true;
}

- (void)textFieldDidEndEditing:(UITextField *)textField {
    NSString* t = fireTextChangeEvent(textField.text);
//    if (t) {
//        textField.text = t;
//    }
}

@end
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void IosTextInput::init(WaseWindow* window, int inputType) SC_NOTHROW {
    //this->window = window;
    
    UIView *view;
    UITextField *textField;
    UITextView *textView;
    switch (inputType) {
        case 1://inputTypeText
            textField = [[UITextField alloc]init];
            view = textField;
            break;
        case 4://inputTypeIntNumber
            textField = [[UITextField alloc]init];
            view = textField;
            textField.keyboardType = UIKeyboardTypePhonePad;
            break;
        case 5://inputTypeFloatNumber
            textField = [[UITextField alloc]init];
            textField.keyboardType = UIKeyboardTypeDecimalPad;
            view = textField;
            break;
        case 2://TextInput::inputTypePassword:
            textField = [[UITextField alloc]init];
            view = textField;
            textField.secureTextEntry = YES;
            break;
        case 3://TextInput::inputTypeMultiLine:
            textView = [[UITextView alloc]init];
            view = textView;
            break;
        default:
            return;
            break;
    }
    
    EditTextListener *listener = [[EditTextListener alloc]init];
    //listener.neditText = fr_newGlobalRef(env, self);
    if (textField) {
        textField.delegate = listener;
        //[textField addTarget:listener action:@selector(textFieldDidChange:) forControlEvents:UIControlEventEditingChanged];
        textField.returnKeyType = UIReturnKeyDone;
    }
    else {
        textView.delegate = listener;
    }
    
    [window addSubview:view];
    //((VaseWindow*)handle->window).editView = view;
    
    this->textView = view;
    this->isTextField = textField != nil;
    this->delegate = listener;
}

void IosTextInput::close() SC_NOTHROW {
    [this->textView removeFromSuperview];
    if (this->isTextField) {
        UITextField *textView = (UITextField*)this->textView;
        //fireTextChangeEvent(self, textView.text);
        textView.delegate = nil;
    }
    else {
        UITextView *textView = (UITextView*)this->textView;
        //fireTextChangeEvent(self, textView.text);
        textView.delegate = nil;
    }
    //fr_deleteGlobalRef(env, this->delegate.neditText);
    //this->delegate.neditText = NULL;
    this->textView = nil;
    this->delegate = nil;
}

void IosTextInput::setPos(int x, int y, int w, int h) SC_NOTHROW {
    float desityScale = g_screenScle;
    
    textView.frame = CGRectMake(x/desityScale, y/desityScale, w/desityScale, h/desityScale);
}
void IosTextInput::setStyle(waseGraphics::Font& font, float fontSize, waseGraphics::Color textColor, waseGraphics::Color backgroundColor) SC_NOTHROW {

    [textView setBackgroundColor:[UIColor colorWithRed:backgroundColor.r() green:backgroundColor.g() blue:backgroundColor.b() alpha:backgroundColor.a()]];
    
    float desityScale = g_screenScle;
    
    int r = textColor.ir();
    int g = textColor.ig();
    int b = textColor.ib();
    int a = textColor.ia();
    
    if (isTextField) {
        UITextField *textView = (UITextField*)this->textView;
        [textView setFont:[UIFont systemFontOfSize:fontSize/desityScale]];
        
        [textView setTextColor:[UIColor colorWithRed:r/255.0 green:g/255.0 blue:b/255.0 alpha:a/255.0]];
    }
    else {
        UITextView *textView = (UITextView*)this->textView;
        [textView setFont:[UIFont systemFontOfSize:fontSize/desityScale]];

        [textView setTextColor:[UIColor colorWithRed:r/255.0 green:g/255.0 blue:b/255.0 alpha:a/255.0]];
    }
}
void IosTextInput::setText(const char* text) SC_NOTHROW {
    NSString *nstext = [NSString stringWithUTF8String:text];
    if (this->isTextField) {
        UITextField *textView = (UITextField*)this->textView;
        [textView setText:nstext];
    }
    else {
        UITextView *textView = (UITextView*)this->textView;
        [textView setText:nstext];
    }
}
void IosTextInput::setType(int lineNum, bool editable) SC_NOTHROW {
    if (this->isTextField) {
        UITextField *textView = (UITextField*)this->textView;
        [textView setEnabled:editable];
    }
    else {
        UITextView *textView = (UITextView*)this->textView;
        [textView setEditable:editable];
    }
}
void IosTextInput::focus() SC_NOTHROW {
    [this->textView becomeFirstResponder];
}

void IosTextInput::select(int start, int end) SC_NOTHROW {
    if (this->isTextField) {
        UITextField *textView = (UITextField*)this->textView;
        UITextPosition *from = [textView positionFromPosition:textView.beginningOfDocument offset:start];
        UITextPosition *to = [textView positionFromPosition:textView.beginningOfDocument offset:end];
        [textView setSelectedTextRange:[textView textRangeFromPosition:from toPosition:to]];
    }
    else {
        UITextView *textView = (UITextView*)this->textView;
        UITextPosition *from = [textView positionFromPosition:textView.beginningOfDocument offset:start];
        UITextPosition *to = [textView positionFromPosition:textView.beginningOfDocument offset:end];
        [textView setSelectedTextRange:[textView textRangeFromPosition:from toPosition:to]];
    }
}
int IosTextInput::caretPos() SC_NOTHROW {
    if (this->isTextField) {
        UITextField *textView = (UITextField*)this->textView;
        int offset = [textView offsetFromPosition:textView.beginningOfDocument toPosition:textView.selectedTextRange.start];
        return offset;
    }
    else {
        UITextView *textView = (UITextView*)this->textView;
        int offset = [textView offsetFromPosition:textView.beginningOfDocument toPosition:textView.selectedTextRange.start];
        return offset;
    }
}
