# STLContainers

## Intro
I am trying to remake the STL Containers in C++, to improve my memory managment skills aswel as grow a better understanding how most of these containers work. I am also learning how to profile and optimize in this project and improving my skill at writing unit tests. This project is still a work in progress, there are most likely still mistakes in there and I do not advice actually using these containers in a project. If you are looking for a very optimized set of containers take a look at https://github.com/electronicarts/EASTL but be sure to adhere to their copyright policies.

## Vector
I used a bit of a different method for the vector than most implementations use, this is because I make most of my class before I look at other implementations. STL uses iterators to track the beginning and end of their allocated data, while EASTL uses pointers. I just use a pointer to keep track where my data starts and then I use integers to keep track of the size of the vector and the size of the total capacity. I also decided to use memcpy and memmove where possible. This is a more risky and harder approach than just always using move and copy operators/constructors as this requires a good understanding of when classes should be destructed and when they shouldn't be. But it should be more performant as no deep copies get maked when not neccesary and we don't bother leaving the old data in a safe state, we just deallocate it.

##### Unit Testing 
I am currently in the process of writing more unit tests as I found the ones I wrote so far to be somewhat lacking. Because I chose to use memcpy and memmove I really need to focus on the fact that I destruct classes at the correct time. The current unit tests mostly focussed on using the vector with trivially destructable types, so things like int, float, ... . This means I am not 100% sure if the container leaves memory leaks or might call a destructor twice on an element.

##### Benchmarking and Profiling
I am currently benchmarking my vector to the STL vector. I am still looking to find a good method to do this because I want to do it right. Currently I just use a timer which tracks time and then output the elapsed duration to the console. But I'dd like to upgrade this to a timer which actually logs times. This way it will be easier to collect large amounts of data and get more accurate results. The benchmarking I have done comparing push back functions shows mine is consitently faster the the STL version, both when reallocating and not reallocating. This is most likely because STL has a lot of safety checks, even when building in development.


## Future work
Because making a fully functional container, testing it and then profiling takes a lot of time I currenty am planning to not make all the STL containers but only the ones that seem the most interesting. The ones I currently am planning to make are:

- unordered_map
- list

