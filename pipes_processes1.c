#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/wait.h> 

int main() 
{ 
    int fd1[2];  // Pipe 1: P1 -> P2 
    int fd2[2];  // Pipe 2: P2 -> P1 
    pid_t p; 
  
    // Create the pipes 
    if (pipe(fd1) == -1 || pipe(fd2) == -1) 
    { 
        fprintf(stderr, "Pipe Failed\n"); 
        return 1; 
    } 
  
    p = fork(); 
  
    if (p < 0) 
    { 
        fprintf(stderr, "Fork Failed\n"); 
        return 1; 
    } 
    // Parent process (P1) 
    else if (p > 0) 
    { 
        char input_str[100]; 
        char concat_str[200]; 
        char second_input[100];  // To receive second input from P2
        
        // Close unused ends of pipes
        close(fd1[0]);  // Close read end of first pipe
        close(fd2[1]);  // Close write end of second pipe
  
        // Prompt for the initial input string 
        printf("Enter a string to concatenate: ");
        scanf("%s", input_str);
  
        // Write the input string to P2 and close writing end of first pipe 
        write(fd1[1], input_str, strlen(input_str) + 1); 
        close(fd1[1]);  // Close write end after sending
  
        // Wait for P2 to send the concatenated string 
        read(fd2[0], concat_str, sizeof(concat_str));
        
        // Print the concatenated string from P2
        printf("Concatenated string from P2: %s\n", concat_str);

        // Wait for the second input sent by P2
        read(fd2[0], second_input, sizeof(second_input));
        close(fd2[0]);  // Close read end after receiving
  
        // Concatenate the second input with the received concatenated string
        strcat(concat_str, second_input);
  
        // Print the final concatenated result
        printf("Final concatenated string: %s\n", concat_str);
    } 
    // Child process (P2) 
    else
    { 
        char concat_str[200]; 
        char fixed_str[] = "howard.edu"; 
        char second_input[100];
        
        // Close unused ends of pipes
        close(fd1[1]);  // Close write end of first pipe
        close(fd2[0]);  // Close read end of second pipe
  
        // Read the string sent by P1 
        read(fd1[0], concat_str, sizeof(concat_str)); 
  
        // Concatenate "howard.edu" to the received string 
        strcat(concat_str, fixed_str);
  
        // Send the concatenated string back to P1 
        write(fd2[1], concat_str, strlen(concat_str) + 1);
  
        // Prompt for a second string to send to P1 
        printf("Enter a second string to send to P1: ");
        scanf("%s", second_input);

        // Send the second input back to P1
        write(fd2[1], second_input, strlen(second_input) + 1);
  
        // Close pipes after use
        close(fd1[0]);  // Close read end of first pipe
        close(fd2[1]);  // Close write end of second pipe
  
        exit(0);  // End child process
    } 

    return 0; 
}
