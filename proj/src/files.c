#include "files.h"

/** Writes to the file "highscores" the name, score and date of
 *  each one of the players on the learderboard
 */
void write_in_file(FILE* highscores, Player leaderboard[])
{
 	for(int i=0; i<10;i++) 	
	{
		if (leaderboard[i].score == -2000) 
 			return;
		//writes the names on the file
		fprintf(highscores, "%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\n", leaderboard[i].nameC[0],  leaderboard[i].nameC[1], leaderboard[i].nameC[2], leaderboard[i].nameC[3], leaderboard[i].nameC[4], leaderboard[i].nameC[5], leaderboard[i].nameC[6], leaderboard[i].nameC[7], leaderboard[i].nameC[8], leaderboard[i].nameC[9]);
		//writes the scores on the file
		fprintf(highscores, "%d\n", leaderboard[i].score);
		//writes the dates on the file
		fprintf(highscores, "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", (leaderboard[i].dateI.year/10)%10,(leaderboard[i].dateI.year%10),(leaderboard[i].dateI.month/10)%10,(leaderboard[i].dateI.month%10),(leaderboard[i].dateI.day/10)%10,(leaderboard[i].dateI.day%10),
		(leaderboard[i].dateI.hour/10)%10,(leaderboard[i].dateI.hour%10),(leaderboard[i].dateI.minute/10)%10,(leaderboard[i].dateI.minute%10),(leaderboard[i].dateI.second/10)%10,(leaderboard[i].dateI.second%10));
 	}
}

/** Opens and closes the files where the leaderboards will be written to
 *  Returns 1 upon failure on opening/creating the files
 */
int write_highscores_in_files(Player leaderboard_classic[], Player leaderboard_extreme[])
{
	FILE * highscores_classic;
  FILE * highscores_extreme;
	highscores_classic = fopen("/home/lcom/labs/proj/highscores_classic.txt","w");
  highscores_extreme = fopen("/home/lcom/labs/proj/highscores_extreme.txt","w");
  if(highscores_classic != NULL) 
	{
    write_in_file(highscores_classic, leaderboard_classic);
    fclose(highscores_classic);
  }
	else
		return 1;
  if(highscores_extreme != NULL) 
	{
    write_in_file(highscores_extreme, leaderboard_extreme);
		fclose(highscores_extreme);
  }
	else
		return 1;
	return 0;
}

/** Reads from the file "highscores" the name, score and date of
 *  each one of the players on the learderboard and places that data
 *  on the leaderboard array.
 */
void read_file(FILE* highscores, Player leaderboard[]) 
{
	int year_dez, year_uni, month_dez, month_uni, day_dez, day_uni, hour_dez, hour_uni, minute_dez, minute_uni, second_dez, second_uni;
	fseek(highscores, 0, SEEK_END);
 	if (0 == ftell(highscores))
 		return;
	fseek(highscores, 0, SEEK_SET);
 	for(int i=0; i<10;i++)
	{
		if (feof(highscores))
			return;
		//gets names from file
		fscanf(highscores, "%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\n", &leaderboard[i].nameC[0],  &leaderboard[i].nameC[1], &leaderboard[i].nameC[2], &leaderboard[i].nameC[3], &leaderboard[i].nameC[4], &leaderboard[i].nameC[5], &leaderboard[i].nameC[6], &leaderboard[i].nameC[7], &leaderboard[i].nameC[8], &leaderboard[i].nameC[9]);
		for(int j=0;j<10;j++)
		{
			unsigned char* aux =scancode_to_img(leaderboard[i].nameC[j]);
			if (aux==NULL)
				aux=image_load(empty_xpm);
			leaderboard[i].name[j]=aux;
		}
		//gets scores from file		
		fscanf(highscores, "%d\n", &leaderboard[i].score);
		int score=abs(leaderboard[i].score);
		for(int j=0; j<4;j++)
		{
			if(score==0)
			{
		 		leaderboard[i].scoreSP[j]=image_load(empty_xpm);
				continue;
			}
			leaderboard[i].scoreSP[j]=int_to_img(score);
			score /= 10;
		}
		if(leaderboard[i].score<0)
			leaderboard[i].scoreSP[3]=image_load(menos_xpm);
		if(leaderboard[i].score==0)
			leaderboard[i].scoreSP[0]=image_load(zero_xpm);
		//gets dates from file
		fscanf(highscores, "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",&year_dez, &year_uni,&month_dez, &month_uni,&day_dez, &day_uni, &hour_dez, &hour_uni, &minute_dez, &minute_uni, &second_dez, &second_uni);
		leaderboard[i].dateI.year=10*year_dez+year_uni;
		leaderboard[i].dateI.month=10*month_dez+month_uni;
		leaderboard[i].dateI.day=10*day_dez+day_uni;
		leaderboard[i].dateI.hour=10*hour_dez+hour_uni;
		leaderboard[i].dateI.minute=10*minute_dez+minute_uni;
		leaderboard[i].dateI.second=10*second_dez+second_uni;
		leaderboard[i].date[0]=int_to_img(year_dez);
		leaderboard[i].date[1]=int_to_img(year_uni);
		leaderboard[i].date[2]=image_load(ponto_xpm);
		leaderboard[i].date[3]=int_to_img(month_dez);
		leaderboard[i].date[4]=int_to_img(month_uni);
		leaderboard[i].date[5]=image_load(ponto_xpm);
		leaderboard[i].date[6]=int_to_img(day_dez);
		leaderboard[i].date[7]=int_to_img(day_uni);
		leaderboard[i].date[8]=int_to_img(hour_dez);
		leaderboard[i].date[9]=int_to_img(hour_uni);
		leaderboard[i].date[10]=image_load(doispontos_xpm);
		leaderboard[i].date[11]=int_to_img(minute_dez);
		leaderboard[i].date[12]=int_to_img(minute_uni);
		leaderboard[i].date[13]=image_load(doispontos_xpm);
		leaderboard[i].date[14]=int_to_img(second_dez);
		leaderboard[i].date[15]=int_to_img(second_uni);
	}
}

/** Opens and closes the files that contain the information of the leaderboards
 * Returns 1 upon failure in opening the file
 */
int read_highscores_from_files(Player leaderboard_classic[], Player leaderboard_extreme[])
{
  FILE * highscores_extreme;
	FILE * highscores_classic;
	highscores_extreme = fopen("/home/lcom/labs/proj/highscores_extreme.txt" , "r");
	highscores_classic = fopen("/home/lcom/labs/proj/highscores_classic.txt" , "r");
	if(highscores_extreme != NULL) 
	{
	 	read_file(highscores_extreme, leaderboard_extreme);
	 	fclose(highscores_extreme);
	}	
	else 
		return 1;
	if(highscores_classic != NULL) 
	{
	 	read_file(highscores_classic, leaderboard_classic);
	 	fclose(highscores_classic);
	}
	else 
		return 1;
  return 0;
}
