#include <plugin.display.sdl/plugin.h>
#include <lib.core/error.h>
#include <lib.core/threads.h>
#include <lib.app/events.h>
#include <SDL2/SDL.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
        Thread *thread;
        ThreadSignal signal;
        SDL_Window *window;
        bool initialized;
        struct timeval tv;
} Global;

static Global global = {
        .thread = NULL,
        .signal = THREAD_SIGNAL_INITIALIZER,
        .initialized = false,
};

static void worker (Thread *thread);

bool load (void)
{
        if (SDL_Init (SDL_INIT_VIDEO) != 0) {
                error (SystemCall);
                return false;
        }
        SDL_Quit ();
        global.thread = NULL;
        return true;
}

bool unload (void)
{
        if (global.thread) {
                thread_set_exit (global.thread);
                thread_wait (global.thread);
                thread_destroy (global.thread);
        }
        return true;
}

bool display_canvas (Canvas *canvas)
{
        if (!(global.thread = thread_create (&worker, canvas))) {
                error (FunctionCall);
                return false;
        }
        if (!thread_signal_wait (&global.signal)) {
                error (FunctionCall);
                return false;
        }
        return global.initialized;
}

static void worker (Thread *thread)
{
        SDL_Renderer *render;
        SDL_Texture *texture;
        SDL_Rect rectangle = { .x = 0, .y = 0, .w = 0, .h = 0 };
        Canvas *canvas = thread->argument;
        SDL_Event event;
        void *pixels;
        int pitch;
        bool running = true;
        bool changed = false;

        if (SDL_Init (SDL_INIT_VIDEO) != 0) {
                error_code (SystemCall, 1);
                return;
        }
        if (!(global.window = SDL_CreateWindow ("plugin.display.sdl", 
                                                SDL_WINDOWPOS_CENTERED, 
                                                SDL_WINDOWPOS_CENTERED, 
                                                (int)canvas->image.width, 
                                                (int)canvas->image.height, 
                                                SDL_WINDOW_SHOWN))) {
                SDL_Quit ();
                error_code (SystemCall, 2);
                return;
        }
        if (!(render = SDL_CreateRenderer (global.window, -1, 
                                           SDL_RENDERER_ACCELERATED | 
                                           SDL_RENDERER_PRESENTVSYNC))) {
                SDL_DestroyWindow (global.window);
                SDL_Quit ();
                error_code (SystemCall, 3);
                return;
        }
        if (!(texture = SDL_CreateTexture (render, 
                                           SDL_PIXELFORMAT_ABGR8888, 
                                           SDL_TEXTUREACCESS_STREAMING,
                                           (int)canvas->image.width, 
                                           (int)canvas->image.height))) {
                SDL_DestroyRenderer (render);
                SDL_DestroyWindow (global.window);
                SDL_Quit ();
                error_code (SystemCall, 4);
                return;
        }
        global.initialized = true;
        if (!thread_signal (&global.signal)) {
                SDL_DestroyTexture (texture);
                SDL_DestroyRenderer (render);
                SDL_DestroyWindow (global.window);
                SDL_Quit ();
                error (FunctionCall);
                return;
        }
        while (running) {
                SDL_Delay (10);
                SDL_PollEvent (&event);
                if (event.type == SDL_QUIT ||
                    event.type == SDL_KEYDOWN ||
                    event.type == SDL_MOUSEBUTTONDOWN) {
                        running = false;
                        continue;
                }
                thread_lock (&canvas->lock);
                if ((changed = canvas->changed)) {
                        rectangle.x = (int)canvas->changed_from.x;
                        rectangle.y = (int)canvas->changed_from.y;
                        rectangle.w = (int)(canvas->changed_to.x - canvas->changed_from.x) + 1;
                        rectangle.h = (int)(canvas->changed_to.y - canvas->changed_from.y) + 1;
                        SDL_RenderClear (render);
                        SDL_LockTexture (texture, NULL, &pixels, &pitch);
                        while (rectangle.h != 0) {
                                memcpy (&((Color *)pixels)[(rectangle.y * (int)canvas->image.width) + rectangle.x],
                                        &canvas->image.map[(rectangle.y * (int)canvas->image.width) + rectangle.x],
                                        (size_t)rectangle.w * sizeof (Color));
                                rectangle.h--;
                                rectangle.y++;
                        }
                        SDL_UnlockTexture (texture);
                        canvas->changed = false;
                }
                thread_unlock (&canvas->lock);
                if (changed) {
                        SDL_RenderCopy (render, texture, NULL, NULL);
                        SDL_RenderPresent (render);
                }
        }
        SDL_DestroyTexture (texture);
        SDL_DestroyRenderer (render);
        SDL_DestroyWindow (global.window);
        SDL_Quit ();
        app_event_exit ();
}
