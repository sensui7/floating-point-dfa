/* Desc:
 * DFA-based program for floating point numbers
 * DFA design has 9 states with the language {+, -, d, ., e, E}
 * where d is a number from the set {0,1,2,3,4,5,6,7,8,9}
 *
 * DFA designed has 9 total states (q0 through q8).
 * q0 = starting state.
 * F = {q3, q4, q5, q8}, or accepting states.
 */

#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

// Code snippet taken from
// https://goo.gl/1rJhpS to handle empty file
// case before even reading it.
static int checkIfEmpty(char ** fileName)
{
    struct stat stat_record;

    if (stat(fileName[1], &stat_record))
    {
        // resource: https://www.tutorialspoint.com/c_standard_library/c_function_strerror.htm
        printf("%s\n", strerror(errno));
        return -1;
    }
    else if (stat_record.st_size <= 1)
    {
        printf("File is empty\n");
        return -1;
    }
   
    return 1;
}

// Helper function to simplify the DFA starting point
static int q0(char toConsume)
{
    if (isdigit(toConsume)) return 2; // move to q2

    switch(toConsume)
    {
        case '+':
            return 1; // move to q1
        case '-':
            return 1; // move to q1
        case '.':
            return 3; // move to q3
    }

    return -1;
}

int main(int argc, char ** argv)
{
    if (argc != 2)
    {
        printf("Expected a filename as the 2nd argument\n");
        return -1;
    }

    if (checkIfEmpty(argv) == -1)
    {
        return -1;
    }
    
    FILE *fp;
    char line[100];
    int i, deadFlag = 0;

    fp = fopen(argv[1], "r");

    // No need to call fclose() since no file was opened
    if (!fp)
    {
        printf("File pointer was NULL\n");
        return -1;
    }

    // Algorithm for DFA begins here
    while (fgets (line, 100, fp) != NULL)
    {
        i = 0;
        printf("String: %s", line);
        int q0state = q0(line[i]);

        switch(q0state)
        {
            case 1:
                ++i;

            q1state:
                if (isdigit(line[i]))
                {
                    goto q2state;
                }
                else if (line[i] == '.')
                {
                    ++i;
                }
                else
                {
                    deadFlag = 1;
                    break;
                }

            case 2:
                q2state:
                ++i;

                // q2 loop same state
                while (isdigit(line[i]))
                {
                    ++i;
                }

                if (line[i] == '\n')
                {
                    break;
                }

                // in q2 right now, thinking of where to go
                if (line[i] == '.')
                {
                    q4state:

                    if (line[i] == '\n')
                    {
                        break;
                    }
                    ++i;

                    // move from q4 to q5
                    if (isdigit(line[i]))
                    {
                        ++i;

                        while (isdigit(line[i]))
                        {
                            ++i;
                        }

                        if (line[i] == '\n')
                        {
                            break;
                        }

                        // move from q5 to q6
                        if (line[i] == 'e' || line[i] == 'E')
                        { 
                            goto q6state;
                        }
                        else
                        {
                            deadFlag = 1;
                            break;
                        }
                    }
                
                    if (line[i] == '\n')
                    {
                        break;
                    }

                    //currently in q4 checking for dead state
                    if (line[i] != 'e' && line[i] != 'E')
                    {
                         deadFlag = 1;
                         break;
                    }
                    else
                    {
                        goto q6state; 
                    }
                }
                else if (line[i] == 'e' || line[i] == 'E') // q4 to q6
                {
                    q6state:
                    ++i;

                    // in q7 state now
                    if (line[i] == '+' || line[i] == '-')
                    {
                        ++i;
                        // in q8 state
                        if (isdigit(line[i]))
                        {
                            ++i;
                            q8state:

                            // loop on all the digits inside q8
                            while (isdigit(line[i]))
                            {
                                ++i;
                            }

                            // reached the ending of the string
                            if (line[i] != '\n')
                            {
                                deadFlag = 1;
                                break;
                            }
                        }
                        else
                        {
                            deadFlag = 1;
                            break;
                        }
                    }
                    else if (isdigit(line[i]))
                    {
                        ++i;
                        goto q8state;
                    }
                    else
                    {
                        deadFlag = 1;
                        break;
                    }
                }
                else
                {
                    deadFlag = 1;
                    break;
                }

                break;
            case 3:
                ++i;

                if (isdigit(line[i]))
                {
                    goto q4state;
                }
                else
                {
                    deadFlag = 1;
                    break;
                }

            case -1:
                printf("RESULT: REJECTED\n");
                printf("---------------\n\n");
                continue;
        }

        if (deadFlag == 1)
        {
            deadFlag = 0;
            printf("RESULT: REJECTED\n");
            printf("---------------\n\n");
            continue;
        }
        else if (deadFlag == 0)
        {
            printf("RESULT: ACCEPTED\n");
            printf("---------------\n\n");
            continue;
        }
    }
    
    return 0;
}
