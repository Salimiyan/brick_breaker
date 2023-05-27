#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <cmath>

#define WIDTH 620
#define HEIGHT 720
#define SPEED 9
#define FONT_SIZE 22
#define BALL_SPEED 18
#define SIZE 16
#define COL 7
#define ROW 5
#define PI 3.14
#define SPACING 16

SDL_Window *window;
SDL_Renderer *renderer;
TTF_Font *font;
bool running, random_mode = true;
SDL_Color color;
SDL_Rect paddle, ball, lives, brick,score,scoreword,livesword; /* rectangle */
float velY, velX;
int liveCount; /* joon ha */
int scoreCount;
std::string scoretext="Score:";
std::string livestext="Lives:";
bool bricks[ROW*COL]; /* ajor ha */


void resetBricks() {
	if(random_mode)
		for(int i=0; i<COL*ROW; i++)
			bricks[i]=rand()%2;
	else
		for(int i=0; i<COL*ROW; i++)
			bricks[i]=true;
	liveCount=3; /* tedad joon ha ro barabar 3 gharar mide */
	scoreCount=0;
	paddle.x=(WIDTH/2)-(paddle.w/2); /* x paddel ro set mikone */
	ball.y=paddle.y-(paddle.h*4); /* y toop ro set mikone */
	ball.x=WIDTH/2-(SIZE/2); /* x toop ro set mikone */
	velY=BALL_SPEED/2;
	velX=0;
}

/* x y ajor ha */
void setBricks(int i) {
	brick.x=(((i%COL)+1)*SPACING)+((i%COL)*brick.w)-(SPACING/2);
	brick.y=brick.h*3+(((i%ROW)+1)*SPACING)+((i%ROW)*brick.h)-(SPACING/2);
}

//neshan dadan matn dar sfhe
void write(std::string text, int x, int y) {
	SDL_Surface *surface;
	SDL_Texture *texture;
	const char* t=text.c_str();
	surface = TTF_RenderText_Solid(font, t, color);
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	lives.w=surface->w;
	lives.h=surface->h;
	lives.x=x;
	lives.y=y;
	SDL_FreeSurface(surface);
	SDL_RenderCopy(renderer, texture, NULL, &lives);
	SDL_DestroyTexture(texture);
}

void update() {
	if(liveCount<=0) resetBricks();
	if(SDL_HasIntersection(&ball, &paddle)) { /* check mikonad ke toop ba paddle barkhord dashteh bashad */
		double rel=(paddle.x+(paddle.w/2))-(ball.x+(SIZE/2));
		double norm=rel/(paddle.w/2);
		double bounce = norm * (5*PI/12);
		velY=-BALL_SPEED*cos(bounce);
		velX=BALL_SPEED*-sin(bounce);
	}
	if(ball.y<=0) velY=-velY; //agar toop be balaye safhe khord, jahat toop aks besheh.
	if(ball.y+SIZE>=HEIGHT) {
		velY=-velY;    //bad az barkhord ba zamin jahat toop avaz shavad va az joon ha yeki kam shavad
		liveCount--;
	}
	if(ball.x<=0 || ball.x+SIZE>=WIDTH) velX=-velX;
	ball.x+=velX; /* toop ra ja be ja kon */
	ball.y+=velY; /* toop ra ja be ja kon */
	if(paddle.x<0) paddle.x=0; //nemizarad ke paddle az samt chap safhe oboor konad.
	if(paddle.x+paddle.w>WIDTH) paddle.x=WIDTH-paddle.w; //nemizarad ke paddle az samt rast safhe oboor konad.

	bool reset=1;
	for(int i=0; i<COL*ROW; i++) { /* 35 ajor ro rasm kon */
		setBricks(i);
		if(SDL_HasIntersection(&ball, &brick) && bricks[i]) { /* agr toop be ajor ha khorde bashad, an ajor ra false mikonad */
			bricks[i]=0;
			if(ball.x >= brick.x || ball.x <= brick.x) velX=-velX; /* jahat toop ra avaz mikonad */
			if(ball.y <= brick.y || ball.y >= brick.y) velY=-velY;
			scoreCount++;
		}
		if(bricks[i]) reset=0; /* agr ajori monde bashad bazi ra edame bede */
	}
	if(reset) resetBricks(); /* You win */
}
//ja be ja kardan paddle ba keyboard
void input() {
	SDL_Event e;
	const Uint8 *keystates = SDL_GetKeyboardState(NULL);
	while(SDL_PollEvent(&e))// barresi mikonad ke event mojood hast ya na
		if(e.type==SDL_QUIT)
			running=false; /* close */
	if(keystates[SDL_SCANCODE_ESCAPE]) running=false; /* close */
	if(keystates[SDL_SCANCODE_LEFT]) paddle.x-=SPEED; /* chap */
	if(keystates[SDL_SCANCODE_RIGHT]) paddle.x+=SPEED; /* rast */
	if(keystates[SDL_SCANCODE_TAB]) liveCount = 9;
	if(keystates[SDL_SCANCODE_R]) {random_mode = true; resetBricks();} // random mode
	if(keystates[SDL_SCANCODE_C]) {random_mode = false; resetBricks();} // classic mode
}

void render() {
	// tenary
	/*
	if(random_mode)
		std::string modetext = "random";
	else
		std::string modetext = "classic"; 
	*/
	std::string modetext = random_mode ? "random" : "classic";
	
	SDL_SetRenderDrawColor(renderer, 000, 000, 000, 255); // Background color RGB
	SDL_RenderClear(renderer); //Stop Coloring

	SDL_Delay((1000/60));

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderFillRect(renderer, &paddle); /* rang kardan paddle */
	SDL_RenderFillRect(renderer, &ball); /* rang kardan toop */

	write(scoretext + std::to_string(scoreCount), WIDTH/2+5*FONT_SIZE+10, FONT_SIZE); /* neshan dadan joon ha ro safeh */
	write(modetext, WIDTH/2-(FONT_SIZE*modetext.length()/2), FONT_SIZE);
	write(livestext + std::to_string(liveCount), WIDTH/2-13*FONT_SIZE,FONT_SIZE);


	for(int i=0; i<COL*ROW; i++) { /* rang kardan ajoor ha */
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		if(i%2==0)SDL_SetRenderDrawColor(renderer, 0,255, 0, 255);
		if(i%3==0)SDL_SetRenderDrawColor(renderer, 128,128, 255, 255);
		if(bricks[i]) {
			setBricks(i);
			SDL_RenderFillRect(renderer, &brick);
		}
	}

	SDL_RenderPresent(renderer);//emal kardan rang ha roye safhe
}


int main(int argv, char** args) {
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0) std::cout << "Failed at SDL_Init()" << std::endl;
	if(SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer) < 0) std::cout << "Failed at SDL_CreateWindowAndRenderer()" << std::endl;
	SDL_SetWindowTitle(window, "Paranoid"); //Set window title
	TTF_Init();
	font = TTF_OpenFont("font.ttf", FONT_SIZE); /* font ro load mikonim */
	running = 1;  /* bazi ro shoro mikonad */
	color.r=color.g=color.b=255;
	paddle.h=12;
	paddle.w=WIDTH/4; /* tool o arz paddle */
	paddle.y=HEIGHT-paddle.h-32;// faseleh paddle az balaye safhe
	ball.w=ball.h=SIZE; /* tool o arz toop */
	brick.w=(WIDTH-(SPACING*COL))/COL; /* tool ajoor */
	brick.h=22;/*arz ajor*/

	resetBricks();

	while(running) {

		update();
		input();
		render();

	}

	TTF_CloseFont(font);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	TTF_Quit();

	return 0;
}
