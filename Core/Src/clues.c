#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "user_diskio_spi.h"

typedef struct{
    char word[25];
    float dims[300];
} Glove_Vector;

typedef struct{
    float dims[200];
} Dict_Vector;

int get_rand_num(void){
	while(1){
		if(RNG->SR & RNG_SR_DRDY ){
			return RNG->DR;
		}
	}
	return 0;

}

unsigned short pick_index(float * arr, unsigned short len, unsigned short highest){
    //Find the index of the highest/lowest float in an array
    float val = arr[0];
    unsigned short ret_idx = 0;
    for(unsigned short idx = 1; idx < len; idx += 1){
        if(highest){
            if(arr[idx] > val){
                val = arr[idx];
                ret_idx = idx;
            }
        }
        else{
            if(arr[idx] < val){
                val = arr[idx];
                ret_idx = idx;
            }
        }
        //printf("Idx: %d Score: %f, Highest:%f\n", idx, arr[idx], val);
    }
    return ret_idx;
}

unsigned short is_subset(char * w1, char * w2){
	//LUXURY MODIFICATION: MAKE SURE 5 OF SAME LETTER IN A ROW -> INVALID CLUE
    //Compute if W1 is a subset of W2 
    //Or the reverse is true
    unsigned short l1 = 0;
    unsigned short l2 = 0;
    while(w1[l1++] != '\0');
    while(w2[l2++] != '\0');
    l1 -= 1;
    l2 -= 1;
    unsigned short dist = (l1 > l2)?l1-l2:l2-l1;
    for(unsigned short idx = 0; idx <= dist; idx += 1){
        unsigned short same = 1;
        if(l1>l2){
            for(unsigned short jdx = 0; jdx < l2; jdx += 1){
                if(w2[jdx] != w1[jdx + idx]){
                    same = 0;
                    break;
                }
            }
        }
        else{
            for(unsigned short jdx = 0; jdx < l1; jdx += 1){
                //printf("%c%c ", w1[jdx], w2[jdx]);
                if(w1[jdx] != w2[jdx + idx]){
                    same = 0;
                    break;
                }
            }
        }
        if(same){
            return 1;
        }
    }
    return 0;
}

void shuffle(unsigned short * board_idx){
    for(unsigned short idx = 0; idx < 2; idx += 1){
        for(unsigned short jdx = 0; jdx < 250; jdx += 1){
        	//USE TRUE_RNG HERE
            unsigned short rnd = (unsigned short)(get_rand_num() % 250);
            unsigned short tmp = board_idx[rnd];
            board_idx[rnd] = board_idx[jdx];
            board_idx[jdx] = tmp;
        }
    }
}

float cosine_similarity(float * v1 , float *v2 , unsigned short len)
{
    float dot = 0.0, denom_a = 0.0, denom_b = 0.0 ;
     for(unsigned short idx = 0; idx < len; idx +=1) {
        dot += v1[idx] * v2[idx] ;
        denom_a += v1[idx] * v1[idx] ;
        denom_b += v2[idx] * v2[idx] ;
    }
    return dot / (sqrt(denom_a) * sqrt(denom_b)) ;
}

int same(char * w1, char * w2){
    //printf("%s, %s\n", w1, w2);
    unsigned short l1 = 0;
    unsigned short l2 = 0;
    while(w1[l1++] != '\0');
    while(w2[l2++] != '\0');
    l1 -= 1;
    l2 -= 1;
    if(w1[0] == w2[0] && w1[1] == w2[1]){
        //printf("%s %s \n", w1, w2);
    }
    if(l1 != l2){
        return 0;
    }
    return (is_subset(w1, w2) == 1);
}

float glove_similarity(Glove_Vector glove1, Glove_Vector glove2){
    return cosine_similarity(glove1.dims, glove2.dims, 300);
}

float dict_similarity(Dict_Vector dict1, Dict_Vector dict2){  
    return cosine_similarity(dict1.dims, dict2.dims, 200);
}

float vector_score(Glove_Vector glove1, Dict_Vector dict1, Glove_Vector glove2, Dict_Vector dict2){
    float glove_score = glove_similarity(glove1, glove2);
    float dict_score = dict_similarity(dict1, dict2);

	return glove_score + dict_score;
}

void gen_board(unsigned short * board_idx, Glove_Vector ** board_glove, Dict_Vector ** board_dict, int num_rounds){
    //Take all possible board words, randomly select 16 to be board words
    //The first two are the targets
    shuffle(board_idx);
//    printf("C1\n");

	//GATHER ALL BOARD VECTOR MATERIALS
    for(unsigned short gather_idx = 0; gather_idx < 16 * num_rounds; gather_idx += 1){
    	char word[25];
    	float * vec_floats;
    	vec_floats = read_vec(board_idx[gather_idx], word);
    	for(unsigned short wi = 0; wi < 25; wi += 1){
        	board_glove[gather_idx / 16][gather_idx % 16].word[wi] = word[wi];
    	}
    	//free(word);
    	for(unsigned short move_idx = 0; move_idx < 300; move_idx += 1){
    		//SAVE WORD TO GLOVE VEC TOO
    		board_glove[gather_idx / 16][gather_idx % 16].dims[move_idx] = vec_floats[1 + move_idx];
    		if(move_idx < 200){
    			board_dict[gather_idx/16][gather_idx%16].dims[move_idx] = vec_floats[301 + move_idx];
    		}
    	}
    	free(vec_floats);
    }

    //Vector Scores for each board comparing to first target
//    float ** scores = malloc(sizeof(*scores) * num_rounds);
    float scores[1][15];
    for(unsigned short brd = 0; brd < num_rounds; brd += 1){
        for(unsigned short idx = 1; idx < 16; idx += 1){
			float scr = vector_score(board_glove[brd][0], board_dict[brd][0], board_glove[brd][idx], board_dict[brd][idx]);
            scores[brd][idx - 1] = scr;
        }
    }
//    printf("C2\n");

    //PICK SECOND TARGET INDEXES
    unsigned short stargets[1];
    for(unsigned short b = 0; b < num_rounds; b += 1){
        stargets[b] = pick_index(scores[b], 15, 1);
    }

    //COMPILE BOARDS
    for(unsigned short b = 0; b < num_rounds; b += 1){
//        Glove_Vector * board_glove2 = malloc(sizeof(*board_glove) * 16);
//        Dict_Vector * board_dict2 = malloc(sizeof(*board_dict) * 16);
    	Glove_Vector board_glove2[16];
    	Dict_Vector board_dict2[16];
        board_glove2[0] = board_glove[b][0];
        board_glove2[1] = board_glove[b][stargets[b] + 1];
        board_dict2[0] = board_dict[b][0];
        board_dict2[1] = board_dict[b][stargets[b] + 1];
        int append = 2;
        for(int idx = 1; idx < 16; idx += 1){
            if(stargets[b] + 1 != idx){
            	board_glove2[append] = board_glove[b][idx];
            	board_dict2[append++] = board_dict[b][idx];
            }
        }
//        free(board_glove[b]);
//        free(board_dict[b]);
//        free(scores[b]);
        board_glove[b] = board_glove2;
        board_dict[b] = board_dict2;
    }

    // for(int i = 0; i < 16; i += 1){
    //     if(same(all_glove[brd_idx[i]].word, board[i]) == 0){
    //         printf("ALARM: %s, %s, brd_idx[i] %d, i %d\n", all_glove[brd_idx[i]].word, board[i], brd_idx[i], i);
    //     }
    //     board_idx[i] = brd_idx[i];
    // }
//    free(stargets);
//    free(scores);
    free(board_idx);
}

int is_valid_clue(Glove_Vector clue, Glove_Vector * board){
    //IN FUTURE: CONSIDER ADDING STEMMER
    for(int idx = 0; idx < 16; idx += 1){
        if(is_subset(board[idx].word, clue.word)){
            return 0;
        }
    }
    return 1;
}

void get_possibles(Glove_Vector ** board_glove, Dict_Vector ** board_dict, float ** possible_scores, unsigned short ** possible_idx, int num_rounds){
    //In the future, make this a HEAP or something faster
    //Consider shift to ALT

	for(unsigned short idx = 0; idx < 500; idx += 1){
        //IMPORTANT: IN THE FUTURE, this will need to be read directly from SD, and FAST
    	Glove_Vector clue_glove;
        Dict_Vector clue_dict;
    	char word[25];
    	float * vec_floats = read_vec(idx, word);
    	float clue_doc_freq = vec_floats[0];
    	for(unsigned short wi = 0; wi < 25; wi += 1){
        	clue_glove.word[wi] = word[wi];
    	}
//    	free(word);
    	for(unsigned short move_idx = 0; move_idx < 300; move_idx += 1){
			//SAVE WORD TO GLOVE VEC TOO
			clue_glove.dims[move_idx] = vec_floats[1 + move_idx];
			if(move_idx < 200){
				clue_dict.dims[move_idx] = vec_floats[301 + move_idx];
			}
		}
		free(vec_floats);


        for(int b = 0; b < num_rounds; b += 1){
            if(is_valid_clue(clue_glove, board_glove[b]) == 0){
                continue;
            }
            float score = vector_score(clue_glove, clue_dict, board_glove[b][0], board_dict[b][0]);
            score += vector_score(clue_glove, clue_dict, board_glove[b][1], board_dict[b][1]);
            score -= clue_doc_freq;
            unsigned short insert_idx = 0;
            for(unsigned short back = (idx<200)?idx:199; back < 51000; back -= 1){
                //Figure out where to insert based on score
                if(possible_scores[b][back] > score){
                    insert_idx = back + 1;
//                    if(insert_idx < 200){
//                        //printf("Possible IDX: %d, Insert IDX: %d \n", idx, insert_idx);
//                    }
                    break;
                }
            }
            float score_pass = -9999;
            unsigned short idx_pass = 60000;
            unsigned short save_idx = 60000;
            if(insert_idx < 200){
                if(possible_idx[b][insert_idx] != -1){
                    score_pass = possible_scores[b][insert_idx];
                    save_idx = possible_idx[b][insert_idx];
                    idx_pass = insert_idx + 1;
                }
                possible_scores[b][insert_idx] = score;
                possible_idx[b][insert_idx] = idx;
            }
            //Pass Everything along
            while(idx_pass < 200){
                float ftemp = possible_scores[b][idx_pass];
                unsigned short itemp = possible_idx[b][idx_pass];
                possible_scores[b][idx_pass] = score_pass;
                possible_idx[b][idx_pass] = save_idx;
                score_pass = ftemp;
                save_idx = itemp;
                idx_pass += 1;
            }


        }
    }
}

float overlap_score(Glove_Vector * board_gloves, Dict_Vector * board_dicts, unsigned short clue_idx){
    //WILL BE DONE BY A READ ON THE FLY
    Glove_Vector clue_glove;
    Dict_Vector clue_dict;
    char * word = malloc(sizeof(*word) * 25);
    float * vec_floats = read_vec(clue_idx, word);
	for(unsigned short move_idx = 0; move_idx < 300; move_idx += 1){
		//SAVE WORD TO GLOVE VEC TOO
		clue_glove.dims[move_idx] = vec_floats[1 + move_idx];
		if(move_idx < 200){
			clue_dict.dims[move_idx] = vec_floats[301 + move_idx];
		}
		if(move_idx < 25){
			clue_glove.word[move_idx] = word[move_idx];
		}
	}
	free(word);
	free(vec_floats);

    //Calculate An Overlap Score
    float max_glove = glove_similarity(clue_glove, board_gloves[2]);
    float max_dict = dict_similarity(clue_dict, board_dicts[2]);
    for(unsigned short idx = 2; idx < 16; idx += 1){
        float glove = glove_similarity(clue_glove, board_gloves[idx]);
        float dict = dict_similarity(clue_dict, board_dicts[idx]);
        if(dict > max_dict){
            max_dict = dict;
        }
        if(glove > max_glove){
            max_glove = glove;
        }
    }
    return 0.5 * (max_glove + max_dict);
}

unsigned short * get_clues(Glove_Vector ** board_glove, Dict_Vector ** board_dict, int num_rounds){
    //consider shift to ALT
    //SETUP POSSIBLE CLUE TRACKING INFOR
    unsigned short ** possible_idx = malloc(sizeof(*possible_idx) * num_rounds);
    float ** possible_scores = malloc(sizeof(*possible_scores) * num_rounds);
    for(int b = 0; b < num_rounds; b += 1){
        possible_idx[b] = malloc(sizeof(**possible_idx) * 200);
        possible_scores[b] = malloc(sizeof(**possible_scores) * 200);
        for(int fill = 0; fill < 200; fill += 1){
            possible_idx[b][fill] = -1;
            possible_scores[b][fill] = -9999;
        }
    }
//    printf("Checkpoint #5\n");

    //GET POSSIBLE CLUES AND THEIR SCORES
    get_possibles(board_glove, board_dict, possible_scores, possible_idx, num_rounds);
//    printf("Checkpoint #6\n");

    //FIND THE BEST CLUE FOR EACH BOARD
    unsigned short * ret_idx = malloc(sizeof(*ret_idx) * num_rounds);
    for(unsigned short b = 0; b < num_rounds; b += 1){
        unsigned short best_idx = possible_idx[b][0];
        float best_score = possible_scores[b][0] - overlap_score(board_glove[b], board_dict[b], possible_idx[b][0]);
        for(int idx = 1; idx < 200; idx += 1){
            float score = possible_scores[b][idx] - overlap_score(board_glove[b], board_dict[b], possible_idx[b][idx]);
            if(score > best_score){
                best_score = score;
                best_idx = possible_idx[b][idx];
            }
        }
        ret_idx[b] = best_idx;
    }

//    for(int b = 0; b < num_rounds; b += 1){
//        for(int idx = 0; idx < 16; idx += 1){
//            save_scores[b][idx] = vector_score(board_glove[b][idx], board_dicts[b][idx], all_glove[ret_idx[b]], all_dict[ret_idx[b]]);
//        }
//    }

    //FREES
    for(int b = 0; b < num_rounds; b += 1){
        free(board_dict[b]);
        free(possible_scores[b]);
        free(possible_idx[b]);
    }
    free(board_dict);
    free(possible_scores);
    free(possible_idx);

    return ret_idx;
}

//void read_doc_freq(char * filename, float * all_doc_freq){
//    FILE * file = fopen(filename, "r");
//    float idf_lower_bound = 0.0006;
//    for(int idx = 0; idx < 50000; idx += 1){
//        for(char ch = fgetc(file); ch != ' '; ch = fgetc(file));
//        //Then read the number
//        float num = 0;
//        for(char ch = fgetc(file); ch != '\n'; ch = fgetc(file)){
//            num *= 10;
//            num += ch - '0';
//        }
//        num = 1.0 / num;
//        if(num < idf_lower_bound){
//            num = 1;
//        }
//        all_doc_freq[idx] = num;
//        // printf("Word: %s, Score: %f\n", string, num);
//        // if(idx == 100){
//        //     break;
//        // }
//    }
//    fclose(file);
//}

void play_round(unsigned short * clues, Glove_Vector ** board_glove, int num_rounds){
	//HAS TO BE CHANGED ENTIRELY TO PLAY ON DEVICE AND NOT TERMINAL
//    for(int b = 0; b < num_rounds; b += 1){
//        printf("Here are your board words for board # %d:\n", b);
//        for(int idx = 0; idx < 4; idx += 1){
//            for(int jdx = 0; jdx < 4; jdx += 1){
//                printf("%s ", all_glove[boards_idx[b][idx*4 + jdx]].word);
//            }
//            printf("\n");
//        }
//        printf("\nHere is your clue: %s\n\n\n", all_glove[clues[b]].word);
//        printf("Here are your board word vector scores:\n");
//        for(int idx = 0; idx < 4; idx += 1){
//            for(int jdx = 0; jdx < 4; jdx += 1){
//                printf("%f ", save_scores[b][idx*4 + jdx]);
//            }
//            printf("\n");
//        }
//        printf("\n\n\n\n\n");
//    }
}

//Glove_Vector * read_glove_2(char * filename, Glove_Vector * all_glove){
//    FILE * file = fopen(filename, "rb");
//    //Read all 50000 Vectors
//    for(int vec_idx = 0; vec_idx < 50000; vec_idx += 1){
//        //First, read the word
//        int ch_idx = 0;
//        Glove_Vector vec = {.word = "\0", .dims = {0, 0}};
//        char ch = '\0';
//        fread(&ch, sizeof(ch), 1, file);
//        while(ch != '\0'){
//            //printf("%c", ch);
//            vec.word[ch_idx++] = ch;
//            fread(&ch, sizeof(ch), 1, file);
//            //printf("\n\nRead: %d\n\n", fread(&ch, sizeof(ch), 1, file));
//        }
//        vec.word[ch_idx] = ch;
//        if(vec_idx % 1000 == 0){
//            //printf("Idx: %d, Word: %s\n", vec_idx, vec.word);
//        }
//
//        // for(int j = 0; j < 1200; j += 1){
//        //     printf("%d", fgetc(file));
//        // }
//        //printf("Fread: %d, Feof: %d, Ferror: %d", fread(vec.dims, sizeof(float), 300, file), feof(file), ferror(file));
//        fread(vec.dims, sizeof(float), 300, file), feof(file), ferror(file);
//        all_glove[vec_idx] = vec;
//        // for(int k = 0; k < 300; k += 1){
//        //     printf("Idx: %d, Val: %f\n", k, vec.dims[k]);
//        // }
//        //print_glove(vec);
//    }
//    fclose(file);
//    return all_glove;
//}

//Dict_Vector * read_dict_2(char * filename, Dict_Vector * all_dict){
//    FILE * file = fopen(filename, "rb");
//    //Read all 50000 Vectors
//    for(int vec_idx = 0; vec_idx < 50000; vec_idx += 1){
//        //First, read the word
//        int ch_idx = 0;
//        Dict_Vector vec = {.word = "\0", .dims = {0, 0}};
//        char ch = '\0';
//        fread(&ch, sizeof(ch), 1, file);
//        while(ch != '\0'){
//            //printf("%c", ch);
//            vec.word[ch_idx++] = ch;
//            fread(&ch, sizeof(ch), 1, file);
//            //printf("\n\nRead: %d\n\n", fread(&ch, sizeof(ch), 1, file));
//        }
//        vec.word[ch_idx] = ch;
//        if(vec_idx % 1000 == 0){
//            //printf("Idx: %d, Word: %s\n", vec_idx, vec.word);
//        }
//
//        // for(int j = 0; j < 1200; j += 1){
//        //     printf("%d", fgetc(file));
//        // }
//        //printf("Fread: %d, Feof: %d, Ferror: %d", fread(vec.dims, sizeof(float), 300, file), feof(file), ferror(file));
//        fread(vec.dims, sizeof(float), 200, file), feof(file), ferror(file);
//        all_dict[vec_idx] = vec;
//        // for(int k = 0; k < 300; k += 1){
//        //     printf("Idx: %d, Val: %f\n", k, vec.dims[k]);
//        // }
//        //print_glove(vec);
//    }
//    fclose(file);
//    return all_dict;
//}

//void read_board_2(char * filename, char ** save_board, int * save_idx, int num_board){
//    FILE * file = fopen(filename, "rb");
//    for(int idx = 0; idx < num_board; idx+=1){
//        char * word = malloc(sizeof(*word) * 25);
//        char read_c = '0';
//        int read_idx = 0;
//        while(read_c != '\0'){
//            fread(&read_c, sizeof(char), 1, file);
//            word[read_idx++] = read_c;
//        }
//        unsigned short wordx;
//        fread(&wordx, sizeof(unsigned short), 1, file);
//        save_idx[idx] = wordx;
//        save_board[idx] = word;
//        //printf("Word: %s, Idx: %d\n", word, wordx);
//    }
//    fclose(file);
//}

//int read_rounds(char * arg){
//    int ret = 0;
//    char c = arg[0];
//    int rds_idx = 1;
//    while(c != '\0'){
//        ret *= 10;
//        ret += c - '0';
//        c = arg[rds_idx++];
//    }
//    return ret;
//}

void clues(){
//    int num_rounds = read_rounds(argv[1]);
     int num_rounds = 1;
//    Glove_Vector all_glove[50000];
//    read_glove_2("glovevecs", all_glove);
//    printf("Checkpoint #1\n");
//
//    Dict_Vector all_dict[50000];
//    read_dict_2("dictvecs", all_dict);
//    printf("Checkpoint #2\n");
//    float all_doc_freq[50000];
//    read_doc_freq("docfreq50k.txt", all_doc_freq);
//    printf("Checkpoint #3\n");
   
//    char ** full_board = read_append("all_board_append.txt");
//
//    write_all(all_glove, all_dict, all_doc_freq, full_board);
//
//    free(full_board);

     // char ** all_board = read_board("codewords.txt");
     // write_idxs("allboard", all_board, all_glove, 100);

     unsigned short * board_idx = malloc(sizeof(*board_idx) * 250);
     for(int i = 0; i < 250; i += 1){
    	 board_idx[i] = i;
     }
//     read_board_2("allboard", all_board, board_idx, 100);
    
     Glove_Vector ** board_glove = malloc(sizeof(*board_glove) * num_rounds);
     Dict_Vector ** board_dict = malloc(sizeof(*board_dict) * num_rounds);
     for(unsigned short i = 0; i < 16; i += 1){
    	 board_glove[i] = malloc(sizeof(**board_glove) * 16);
    	 board_dict[i] = malloc(sizeof(**board_dict) * 16);
     }
     // EMPTY TO SAVE GLOVE AND DICT VECTORS IN

     gen_board(board_idx, board_glove, board_dict, num_rounds);
//     printf("Checkpoint #4\n");


//     float ** save_scores = malloc(sizeof(*save_scores) * num_rounds);
//     for(int b = 0; b < num_rounds; b += 1){
//         save_scores[b] = malloc(sizeof(**save_scores) * 16);
//         for(int idx = 0; idx < 16; idx+= 1){
//             save_scores[b][idx] = 0;
//         }
//     }

     unsigned short * clues = get_clues(board_glove, board_dict, num_rounds);

//     printf("Checkpoint #7\n");
     play_round(clues, board_glove, num_rounds);

//
//     printf("Checkpoint #8\n");
//     for(int idx = 0; idx < 100; idx += 1){
//         free(all_board[idx]);
//     }

//     for(int b = 0; b < num_rounds; b += 1){
//         //free(boards_idx[b]);
//         //free(save_scores[b]);
//     }
     free(clues);
//     free(save_scores);
//     free(boards_idx);
}
