Input Format

The first line of the input is 1 or 2 indicating its either white or black who is playing the game. 26 lines follow. Each line contains 1 or 2 integers ( separated by a single space ) indicating the number of checkers on the point and the type of the checker ( 1 for 1st player's checker and 2 for 2nd player's checker)

5 1 indicates that there are 5 checkers of the first player.
6 2 indicates that there are 6 checkers of the second player.
0 indicates that the point has no checkers. The first and the last line are bear off points for 2nd player and the 1st player respectively.
2 lines follow each line indicating the number of coins in the bar for the 1st player and the 2nd player.
The next line contains an integer ( 2 or 4 ) indicating the number of dice rolls played on behalf of player by the computer. 4 both the dice rolled the same number. 2 or 4 lines follow each indicating a number between 1-6 which is the number on the dice.

Output Format

For every possible legal dice roll possible, print a checker move in a new line. Each move has two integers ( start point and end point ).

1 2 a checker is moved from 1 to 2.
-1 5 a checker is removed from the bar and is placed at 5 on a dice roll of 5.
1 0 a coin is being bared off.
Note:-

player 1 moves his checkers from 1 to 24 and later to 25 for bearing off.
player 2 moves his checkers from 24 to 1 and later to 0 for bearing off.
player 1 removes his checkers from bar and places it on the points between 1 to 6.
player 2 removes his checkers from bar and places it on the points between 19 to 24. A dice roll of 1 would place a checker for player 2 from bar to 24, a dice roll of 2 would place it on 23 and so on.
If your code has an input, it is guaranteed that atleast 1 dice roll is legally possible.

Sample Input

1
0
2 1
0
0
0
0
5 2
0
3 2
0
0
0
5 1
5 2
0
0
0
3 1
0
5 1
0
0
0
0
2 2
0
0
0
2
5
4
Sample Output

1 5
12 17
Explanation

Here dice roll is 5, 4. 1st player moves his checker from 1 to 5 and another from 12 to 17.
