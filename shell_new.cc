#include "shell.h"
#include "vgatext.h"

//buffer refresh
//backspace implement
//state_update

//
// initialize shellstate
//
void shell_init(shellstate_t& state){
  state.num_keys = 100;
  state.shell_state = 0x07;
  state.output = 0x0000000000000000;
  state.pnt_buf = -1;
  state.pnt_cmd = -1;
  state.cursor_x = 0;
  state.cursor_y = 0;
  for(int i=0; i<256;i++){
    (state.in_buf[i]).char_val = 0x00;
    (state.in_buf[i]).x = 0;
    (state.in_buf[i]).y = 0;
    if(i<50){
      (state.curr_cmd[i]).char_val = 0x00;
      (state.curr_cmd[i]).x = 0;
      (state.curr_cmd[i]).y = 0;
    }
  }
}

//0x00 - black; 0x07 - white, 0x01 - blue

//
// handle keyboard event.
// key is in scancode format.
// For ex:
// scancode for following keys are:
//
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | keys     | esc |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 |  0 |  - |  = |back|
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | scancode | 01  | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | 0a | 0b | 0c | 0d | 0e |
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | keys     | tab |  q |  w |  e |  r |  t |  y |  u |  i |  o |  p |  [ |  ] |entr|
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | scancode | 0f  | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 1a | 1b | 1c |
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | keys     |ctrl |  a |  s |  d |  f |  g |  h |  j |  k |  l |  ; |  ' |    |shft|
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | scancode | 1d  | 1e | 1f | 20 | 21 | 22 | 23 | 24 | 25 | 26 | 27 | 28 | 29 | 2a |
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//
// so and so..
//
// - restrict yourself to: 0-9, a-z, esc, enter, arrows
// - ignore other keys like shift, control keys
// - only handle the keys which you're interested in
// - for example, you may want to handle up(0x48),down(0x50) arrow keys for menu.
//
void shell_update(uint8_t scankey, shellstate_t& stateinout){

  hoh_debug("Got: "<<unsigned(scankey));
  stateinout.num_keys++;
  if(stateinout.shellstate != 0x00){   //black background and foreground - computation going on
    pnt_cmd++;
    stateinout.curr_cmd[pnt_cmd].char_val = scankey;
    // if(stateinout.pnt_cmd==(-1))  ///////////////////////////CHECK what should be the x and y for a new cmd;

    stateinout.curr_cmd[pnt_cmd].x = stateinout.curr_cmd[pnt_cmd-1].x;
    stateinout.curr_cmd[pnt_cmd].y = stateinout.curr_cmd[pnt_cmd-1].y;
    if(scankey == 0x1c){    //enter is pressed
      //add to in_buf
      for(int i=0; i<=pnt_cmd; i++){
        pnt_buf++;
        stateinout.in_buf[pnt_buf] = stateinout.curr_cmd[i];
      }
      shell_step(stateinout);
      //empty the curr_cmd buffer
      pnt_cmd = -1;
    }
  }
  else{   
    
  }

}

void factorial(int num){
  int ans = 1;
  for(int i=1; i<=num; i++){
    ans = ans*i;
  }
  return ans;
}

void fibonacci(int num){
  int ans;
  if(num==0 || num==1)
    return 1;

  else{
    return(fibonacci(num-1)+fibonacci(num-2));
  }
}


//
// do computation
//
void shell_step(shellstate_t& stateinout){

  //
  //one way:
  // if a function is enabled in stateinout
  //   call that function( with arguments stored in stateinout) ;
//stateinout.args[0] = 5;
//stateinout.args[1] = 5;
  //

  //format:
  //1) "=fi 4" then press enter
  //2) "=fa 10" then press enter

  int i=0;
  int func_id=0; //1-> for fi, 2-> for fa
  while(stateinout.curr_cmd[i]!=0x0f){ //while not equal to space
    if(i==0){
      if(stateinout.curr_cmd[i]!=0x0d){  //not equal to 'f'
        stateinout.errorMsg = "ERROR: Not a valid function call. Only \"=fi\" and \"=fa\" allowed!";
        break;
      }  
    }
    else if(i==1){
      if(stateinout.curr_cmd[i]!=0x21){  //not equal to 'f'
        stateinout.errorMsg = "ERROR: Not a valid function call. Only \"=fi\" and \"=fa\" allowed!";
        break;
      }  
    }
    else if(i==2){
      if(stateinout.curr_cmd[i]!=0x1e)  //its fact probably
        func_id = 2;
      
      else if(stateinout.curr_cmd[i]!=0x17)   //its fib
        func_id = 1;
      else{
        stateinout.errorMsg = "ERROR: Not a valid function call. Only \"fi\" and \"fa\" allowed!";
        break;
      }
    }
    i++;
  }

  int arg = 0;
  if(func_id != 0){  //either fi or fa
    i++;
    int temp = 0;
    while(stateinout.curr_cmd[i]!=0x1c){ //enter
      temp = ((int)stateinout[i]);
      if(temp==1 || temp>11){
        stateinout.errorMsg = "ERROR: Not a valid argument. Only integers allowed!";
        func_id = 0;
        break;
      }
      temp = temp%11;
      if(temp!=0) temp--;

      arg = arg*10 + (temp);
    }
  }  

  //now we have both func type and argument
  int output = 0;
  if(func_id == 1)
    output = fibonacci(arg);
  else if(func_id == 2)
    output = factorial(arg);

  stateinout.output = (uint64_t)(output);
}


//
// shellstate --> renderstate
//
void shell_render(const shellstate_t& shell, renderstate_t& render){

  render.num_keys =  shell.num_keys;
  render.shell_state = shell.shell_state;
  render.output = shell.output;
  for(int i=0; i<256;i++){
    render.out_buf[i] = shell.in_buf[i];
  }
  // etc.
  //
}


//
// compare a and b
//
bool render_eq(const renderstate_t& a, const renderstate_t& b){
  if((a.num_keys == b.num_keys)&&
     (a.shell_state == b.shell_state)&&
     (a.output==b.output)&&
     (a.out_buf == b.out_buf))
       return true;
  return false;
}


static void fillrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void drawrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void drawtext(int x,int y, const char* str, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void drawnumberinhex(int x,int y, uint32_t number, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);

//
// Given a render state, we need to write it into vgatext buffer
//
void render(const renderstate_t& state, int w, int h, addr_t vgatext_base){


  // this is just an example:
  //
  // Please create your own user interface
  //
  // You may also have simple command line user interface
  // or menu based interface or a combination of both.
  //
  drawrect(0,0,80,25,0x00,0x07,80,25,vgatext_base);
  const char* out_str;
  out_str = "Keys Pressed: ";
  drawtext(1,1,out_str,80,0x00,0x07,80,25,vgatext_base);
  drawnumberinhex(15,1,(uint32_t)state.num_keys,80,0x00,0x07,80,25,vgatext_base);
  for(int i=0; i<256 && state.out_buf[i].char_val != 0x00;i++){
    //writecharxy(2+(state.out_buf[i]).x, 3+(state.out_buf[i]).y,(state.out_buf[i]).char_val,0x00,state.shell_state, 80,25,vgatext_base);
    vgatext::writechar(state.out_buf[i].y * 80 + state.out_buf[i].x,state.out_buf[i].char_val,0x00,state.shell_state,vgatext_base);
  } 

}


//
//
// helper functions
//
//

static void writecharxy(int x, int y, uint8_t c, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){
  vgatext::writechar(y*w+x,c,bg,fg,vgatext_base);
}

static void fillrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){
  for(int y=y0;y<y1;y++){
    for(int x=x0;x<x1;x++){
      writecharxy(x,y,0,bg,fg,w,h,vgatext_base);
    }
  }
}

static void drawrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){

  writecharxy(x0,  y0,  0xc9, bg,fg, w,h,vgatext_base);
  writecharxy(x1-1,y0,  0xbb, bg,fg, w,h,vgatext_base);
  writecharxy(x0,  y1-1,0xc8, bg,fg, w,h,vgatext_base);
  writecharxy(x1-1,y1-1,0xbc, bg,fg, w,h,vgatext_base);

  for(int x=x0+1; x+1 < x1; x++){
    writecharxy(x,y0, 0xcd, bg,fg, w,h,vgatext_base);
  }

  for(int x=x0+1; x+1 < x1; x++){
    writecharxy(x,y1-1, 0xcd, bg,fg, w,h,vgatext_base);
  }

  for(int y=y0+1; y+1 < y1; y++){
    writecharxy(x0,y, 0xba, bg,fg, w,h,vgatext_base);
  }

  for(int y=y0+1; y+1 < y1; y++){
    writecharxy(x1-1,y, 0xba, bg,fg, w,h,vgatext_base);
  }
}

static void drawtext(int x,int y, const char* str, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){
  for(int i=0;i<maxw;i++){
    writecharxy(x+i,y,str[i],bg,fg,w,h,vgatext_base);
    if(!str[i]){
      break;
    }
  }
}

static void drawnumberinhex(int x,int y, uint32_t number, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){
  enum {max=sizeof(uint32_t)*2+1};
  char a[max];
  for(int i=0;i<max-1;i++){
    a[max-1-i-1]=hex2char(number%16);
    number=number/16;
  }
  a[max-1]='\0';

  drawtext(x,y,a,maxw,bg,fg,w,h,vgatext_base);
}

