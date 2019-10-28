/*
Alexandro Francisco Marcelo Gonzalez A01021383

01/03/2019
*/

You have download the game: Blackjack. This program runs in C language, make sure you can run C programs in your computer.

In order to use, you should (In linux):

1. Open two terminals within the folder A01021383_blackjack

2. Get your ip from your computer, you can obtain this in your preferences (Mac) or check this post: https://www.linuxtrainingacademy.com/determine-public-ip-address-command-line-curl/

3. In the first terminal you type make to compile the program.

4. In the same terminal as the 3th step, you run the server program as:
    ./server {port}
    (the port is the listening port where the server are listening for a client)
    I recomend you tu put 8989 as the port, therefore:
    ./server 8989

5. In the second terminal opened, you connect as client to the server in order to play. Therefore you type in the terminal:
    ./client {server_ip_address} {port}
    (the server_ip_address is the ip that you get in the step 2 and the port must be the same as the port you put in step 4th)
    For example:
    ./client 192.168.0.4 8989

6. Finally you can play Blackjacl in the client program (5th step), I recommend you to visit these pages if you have questions about how to play Blackjack:
    https://www.bicyclecards.com/how-to-play/blackjack/
    https://www.pagat.com/banking/blackjack.html

7. If you want to stop the server or the client program just type ctrl-c in the terminal where the program is running

Thak you for download the Blackjack game maded by Alexandro Fracisco Marcelo Gonzalez
