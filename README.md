*This project has been created as part of the 42 curriculum by kmalfois*


-------------------------------------------------------------------------------


CODEXION Project
=============


-------------------------------------------------------------------------------
# DESCRIPTION
CODEXION teaches us about the concept of **threads**, **multi-threading** and **execution priority**. \
The goal of this project is to run a set number of threads, executing a cycling routine script composed of different timed tasks, that must take in consideration the state of other threads in the program. \
\
To do so, a revised version of the "*Dining Philosophers*" concept will be used in this exercice. \
Here, **coders** (or threads in our project) will need to **debug**, **refactor** and **compile** their code instead.
Just like philosophers need 2 forks to be able to eat, coders will need to acquire 2 USB dongles to have access to the compiling terminal. The difference with the initial concept being that dongles also have a cooling down timer, making them unusable for a set amount of time after a coder's compilation. \
The final goal is thus for every thread in the program to be able to compile a set amount of times. If a coder can't make a compilation in time, just like philosophers can die from starvation, *coders will be considered burnt out* and the program will shutdown.


-------------------------------------------------------------------------------
# INSTRUCTIONS
The CODEXION program must work with 8 **mandatory** arguments:
- **number_of_coders**: Amount of coders in the circle, and thus the amount of dongles present between each.
- **time_to_burnout**: Time (milliseconds) before a coder's burnout.
- **time_to_compile**: Time (milliseconds) needed by a coder to compile his code.
- **time_to_debug**: Time (milliseconds) needed by a coder to debug his code.
- **time_to_refactor**: Time (milliseconds) needed by a coder to refactor his code.
- **number_of_compiles_required**: Amount of compilations needed per coders for the program to be complete.
- **dongle_cooldown**: Time (milliseconds) for a USB dongle to cooldown and be usable again.
- **scheduler**: Can be set to "fifo" (First In First Out) or "edf" (End of Deadline First).


```shell
#example
./codexion 4 1900 400 200 600 4 100 edf
```


Threads must then display their respective operations while sharing resources until the program is terminated, rather by coder burnout or compilation quota completed for every coders:


![Codexion example](assets/example.png)


-------------------------------------------------------------------------------
# RESOURCES
GeekForGeeks: [Thread Management Functions in C](https://www.geeksforgeeks.org/c/thread-functions-in-c-c/) \
CodeVault: [Short introduction to threads (pthreads)](https://www.youtube.com/watch?v=d9s_d28yJq0)


**AI** has been used for clues, information and code assessment.\
**NO CODE IN THIS PROJECT WAS COPY/PASTED FROM AI MODELS !**


-------------------------------------------------------------------------------
# MAKEFILE


Implemented makefile will possess the following commands:
- **make all**: will run the codexion command
- **make codexion** [$(NAME)]: Compiles the program to create the *codexion* binary.
- **make clean**: Destroys the obj folder, containing .o objects.
- **make fclean**: Executes clean command and destroys the codexion binary executable.
- **make re**: Executes fclean followed by codexion to recompile the program.
- **make fifo**: Executes the program with preset arguments to demonstrate a failing fifo scenario.
- **make edf**: Executes the program with the same preset arguments as "*make fifo*" to demonstrate how the edf schedule model manages priority differently.
- **make val**: Executes the "*make edf*" with valgrind to monitor leaks.
- **make lint**: Executes Norminette.


-------------------------------------------------------------------------------
# PROGRAM
## LIBRARIES
This Codexion program implements and uses all authorized libraries in the subject
```c
// LIBRARIES
# include <stdio.h>     // printf, fprintf
# include <unistd.h>    // write, usleep
# include <stdlib.h>    // malloc, free, atoi
# include <string.h>    // strcmp, strlen, memset
# include <pthread.h>   // pthread functions
# include <sys/time.h>  // gettimeofday
```
## STRUCTURES
S_Config regroups all variables required for the execution of the program
```c
typedef struct s_config
{
   int             nbr_coders;
   int             tt_burnout;
   int             tt_compile;
   int             tt_debug;
   int             tt_refactor;
   int             compiles_req;
   int             dgl_cd;
   int             fifo_edf; // 0:fifo 1:edf
   long            time_start; // timestamp of the simulation's start
   int             sim_status; // boolean for the simulation's status
   pthread_mutex_t lock_sim_status; // mutex for consulting sim_status
   pthread_mutex_t lock_write; // mutex to be allowed to write
   pthread_t       monitor; // monitor thread
   t_dongle        *dongles; // dongles array
   t_coder         *coders; // coders array
}   t_config;
```
S_Coder contains the elements of a coder, including the thread that will be executed.
```c
typedef struct s_coder
{  
   pthread_t       thread;
   int             id; // ID that helps retrieving neighbors info
   long            req_time; // timestamp of the last compile request
   pthread_mutex_t lock_req_time; // request_time mutex
   int             compiled; // amount of compilations done
   pthread_mutex_t lock_compiled; // compiled mutex
   long            last_comp; // timestamp of the last compilation
   pthread_mutex_t lock_last_comp; // last_comp mutex
   t_config        *config; // pointer to associated s_config to retrieve data
   t_dongle        *l_dgl; // pointer to left USB dongle (N)
   t_dongle        *r_dgl; // pointer to right USB dongle (N+1)
}   t_coder;
```
S_Dongle represents a USB dongle.
```c
typedef struct s_dongle
{
   pthread_mutex_t dongle; // mutex that reprents the device
   int             id;
   long            last_released; // last used timestamp
}   t_dongle;
```


## THREADS AND MUTEXES


Threads in C are managed by the ***pthread*** library. \
It supervises the creation and execution of threads and manages the declaration of variables allowing thread entities to communicate with eachothers, ensuring their cohesion.


### pthread_create()
Threads are created using the *pthread_create()* function. \
Once created, the thread will execute their assigned script until an error occurs or the objective is fulfilled.
```c
pthread_create(
   &config->monitor, // Address to thread's allocated memory
   NULL,
   (void *)monitor_script, // Script function that'll be run by the thread
   config // Data passed to the script function
)
```
### pthread_join()
To terminate a thread, we use the *pthread_join()* function
```c
pthread_join(config->monitor, NULL);
```
### pthread_mutex_init()
Threads can communicate using **Mutual Execution** variables.
By locking and unlocking these variables, we ensure that threads do not step on each other during execution. \
```c
pthread_mutex_init(&config->lock_sim_status, NULL);
```


### pthread_mutex_lock()
As an example, we cannot let threads use printf() at will, or info displayed in the terminal will be unreadable. \
To solve this issue, we use a function that **locks** the **lock_write mutex** from s_config before using printf. If the mutex is locked by another thread, it will wait until a lock is possible before proceeding.
```c
pthread_mutex_lock(&self->config->lock_write);
printf("\033[36m%ld\033[0m - Coder %d %s\n", time, self->id, msg);
pthread_mutex_unlock(&self->config->lock_write);
```


### pthread_mutex_trylock()
The *ptrhead_mutex_trylock*() function can also be used if we want to try a mutex lock without necessarily pausing the script execution, allowing us to check if the simulation is still running while trying for example.
```c
if (!pthread_mutex_trylock(&self->r_dgl->dongle))
{
   [...]
}
```

### pthread_mutex_destroy()
Once the program's task is over, mutexes must me deleted to free their allocated memory using *pthread_destroy()*
```c
pthread_mutex_destroy(&config->lock_write);
```

### Blocking cases handled
To prevent the deadlock scenario: when every coder has one dongle they won't release, resulting in every coders' burnout, we use the pthread_mutex_trylock() function. \
In this program, a coder will try to lock the first dongle, if it works it then tries to lock the second dongle, but it this second lock fails, it will release the first and retry the entire process once again. \
This way, no dongles will be held by a coder indefinitely, preventing the deadlock situation.
A final failsafe, in case deadline(edf) or request(fifo) timestamps are identical, will decide which coder is prioritized based on their ID.
```c
static void	coder_compile(t_coder *self)
{
    // Checks if the simulation's still running
	while (check_sim_status(self))
	{
        // priority check
		if (coder_ready(self)) 
		{
            // first dongle lock attempt
			if (!pthread_mutex_trylock(&self->l_dgl->dongle))
			{
                // second dongle lock attempt
                sim_print(self, "Has taken his left dongle", 0);
				if (!pthread_mutex_trylock(&self->r_dgl->dongle))
				{
                    //checks dongles cooldown
                    sim_print(self, "Has taken his right dongle", 0);
					if (!dongle_cooldown(self->l_dgl, self->config->dgl_cd) &&
						!dongle_cooldown(self->r_dgl, self->config->dgl_cd))
					{
                        //success, we compile, release and return
						coder_compilation(self);
						pthread_mutex_unlock(&self->l_dgl->dongle);
						pthread_mutex_unlock(&self->r_dgl->dongle);
						return;
					}
                    // if it fails we release dongle 2
					pthread_mutex_unlock(&self->r_dgl->dongle);
				}
                // release dongle 1
				pthread_mutex_unlock(&self->l_dgl->dongle);
			}
		}
        // micro pause to prevent CPU overload and try again
		usleep(400);
	}
}
```

### Thread synchronization mechanisms
List of mutexes/variables used in this project:
- **(lock_)sim_status**: sim_status can be locked by the monitor to edit the status and signal the end of the simulation.
- **lock_write**: is the mutex to use by coders or the monitor to display a message in the terminal, preventing all parties to try writting at once.
- **(lock_)request_time**: registers the timestamp of a coder's last request for compilation, this will allow the fifo_priority() function to know which coder was the "first in" and give the greenlight to lock a dongle.
- **(lock_)compiled**: The amount of compilations done is edited by coders and read by the monitor, this lock is essential to prevent a coder from editing its counter while the monitor is checking all compilation counts.
- **(lock_)last_comp**: is the lock that helps edf_priority() decide which coder is closest to its deadline for dongle acquisition.
- **dongle**: is the USB device itself, this mutex will be locked by a coder when in use, then unlocked while updating the last_released variable to manage dongle cooldown.


## CODE INFRASTRUCTURE
- **Makefile**: Makefile command file
- **README.md**: Program guide and information
- **[src]**: Contains all .c files
   - **main.c**: main file
   ```c
   int         main(int argc, char *argv[]); // main function
   static int  start_sim(t_config *config); // triggers simulation's start
   static void end_sim(t_config *config); // closes threads once the sim ends
   static void report(t_config *config); // additional report (personal addition)
   ```
   - **parser.c**: Checks arguments values
   ```c
   int         parser(int argc, char *argv[]);
   static int  is_valid_number(char *str); // check if positive integers
   static int  is_valid_scheduler(char *str); //check if "fifo" or "edf"
   ```
   - **initializer.c**: Initialize every structures
   ```c
   int init_config(t_config *config, char *argv[]); // init config struct
   static t_dongle *init_dongles(int nbr_coders); // init dongle struct array in config
   static t_coder  *init_coders(t_config *config, int nbr_coders); // init coder struct array in config
   void    cleanup(t_config *config); // purge allocated memory and destroys mutexes
   ```
   - **monitor.c**: monitor script and tools
   ```c
   void        monitor_script(t_config *config); // monitor routine script
   static int  check_deadlines(t_config *config); // checks all coders deadlines
   static int  check_compiles(t_config *config); // checks all coders compilation counter
   static int  check_deadline(t_coder *coder); // check singular coder deadline
   static int  check_sim_status_monitor(t_config *config); // checks if simulation is still running
   ```
   - **coder.c**: coders routine script
   ```c
   void        coder_script(t_coder *self); // coder routine script
   static void coder_compile(t_coder *self); // try to access mutexes
   static void coder_compilation(t_coder *self); // compilation
   static int  coder_ready(t_coder *self); // checks if compilation scheduler conditions are met
   ```
   - **coder_tools.c**: coders script's tools
   ```c
   int fifo_priority(t_coder *self, int side); // checks priority when fifo
   int edf_priority(t_coder *self, int side); // checks priority when edf
   int dongle_cooldown(t_dongle *dongle, int cooldown); // checks dongle cool down
   ```
   - **utils.c**: utility functions
   ```c
   long    get_time(void); // recovers current time in milliseconds
   void    sim_print(t_coder *self, char* msg, int critical); // print function
   t_coder *get_neighbor(t_coder *coder, int side); // recover neighbors struct data
   int     check_sim_status(t_coder *self) // checks if simulation is still running
   ```
- **[include]**
   - **codexion.h**: Contains libraries, structures and non-static function prototypes
- **[obj]**: Contains object files, can be deleted with the *make clean* command
- **[assets]**: Contains images for the README.md


## EXECUTION


Conceptually, every threads will be running a coder script, that debugs, refactors, and if possible, compile then repeat. \
In parallel, an additional thread, managed by the s_config, will supervise the execution of the program. It will wake up on a regular timer to check if:
- All coders have the right amount of compilations
- One of the coder has exceeded its deadline and burnt out


When one of those conditions are met, the monitor will turn the sim_status variable from s_config to 0, and from here, every threads will cease activity and the routine will be considered finished. \
The program will then perform a cleanup, destroying all mutexes and freeing allocated memory, display a report, then shutdown.


![Codexion logigram](assets/logigram.png)


## CONCLUSION
Playing with threads and understand their inner workings was very interesting. \
The challenge revolved around the way each thread would access key variables, both for reading or editing. \
The concept of priorities also played an important part of this project, to discern how different scheduling methods dictate the behavior of a multi-thread program was pretty insightful to witness first hand.

