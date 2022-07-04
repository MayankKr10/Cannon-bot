#include "iostream"
#include "vector"
#include "map"
#include "unordered_map"
#include "algorithm"
#include "thread"
#include "utility"
#include "limits"
#include "string"
#include "sstream"
#include "float.h"
using namespace std;

class state
{
public:

	vector<vector<short>> board_state;
	short depth,color,ourPlayer,moves;
	tuple<short, pair<short,short>, pair<short,short>> back_move;
	tuple<short, pair<short, short>, pair<short, short>> fro_move;
	vector<pair<short, short>> soldiers;
	vector<pair<short,pair<short, short>>> cannons;
	float eval_value;
	vector<state*> child_states;
	short sattack, snot_attack,cnot_attack,cattack,townhall_attack;

	state();
	state(vector<vector<short>> board, tuple<short, pair<short, short>, pair<short, short>> move, short dep, short col,short oPlayer,float alpha,float beta,short movestill);
	void Generate_childs();
	void print_move();
	void update_moves_count();
	vector<pair<short, short>> get_soldier_moves(pair<short,short>);
	vector<tuple<short, pair<short, short>, pair<short, short>>> get_cannon_moves(pair<short,pair<short, short>>);
	float evaluate(short sattack, short snot_attack, short townhall_attack);
	bool search_sol(short x1, short y1);
	float volaitlity(pair<short,short>);
	short get_val();
	state* get_next_state();
	state* get_next_state(tuple<short,pair<short,short>,pair<short,short>>, float, float);
	vector<vector<short>> get_next_board(tuple<short, pair<short, short>, pair<short, short>>);
	float get_pos_voltil(pair<short, pair<short, short>>);
	void print_board();
	bool adjacent(pair<short, short>);
	vector<pair<short, pair<short, short>>> get_cannon_pos();
	float miniMaxPrune(float alpha, float beta);

};


state::state()
{
	depth = eval_value = color = 0;
	ourPlayer = moves = 0;
	sattack = cattack = 0;
	snot_attack = cnot_attack = townhall_attack = 0;
	board_state = { {-2,-1,-2,-1,-2,-1,-2,-1},
	{0,-1,0,-1,0,-1,0,-1},
	{0,-1,0,-1,0,-1,0,-1},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{1,0,1,0,1,0,1,0},
	{1,0,1,0,1,0,1,0},
	{1,2,1,2,1,2,1,2},
	};


}

state::state(vector<vector<short>> board, tuple<short, pair<short, short>, pair<short, short>> move,short dep,short col,short oPlayer,float alpha,float beta,short movestill)
{
	back_move = move;
	snot_attack = cnot_attack = 0;
	ourPlayer = oPlayer;
	board_state = board;


	moves = movestill;
	if (get<0>(move) == 0)
	{
		//cout<<board_state[get<1>(move).second][get<1>(move).first]<<endl;
		//cout<<board_state[get<2>(move).second][get<2>(move).first]<<endl;
		board_state[get<2>(move).second][get<2>(move).first] = board_state[get<1>(move).second][get<1>(move).first];
		board_state[get<1>(move).second][get<1>(move).first] = 0;
		//print_board();

	}
	else if (get<0>(move) == 1)
	{
		board_state[get<2>(move).second][get<2>(move).first] = 0;

	}

	sattack = cattack = townhall_attack = 0;
	depth = dep;
	color = col;
	if (color == 0)
	{
		for (short i = 0; i < 8; i++)
		{
			for (short j = 0; j < 8; j++)
				if (board_state[i][j] == 1)
					soldiers.push_back(make_pair(j, i));
		}
	}
	else
	{
		for (short i = 0; i < 8; i++)
		{
			for (short j = 0; j < 8; j++)
				if (board_state[i][j] == -1)
					soldiers.push_back(make_pair(j, i));
		}
	}
	//cout<<soldiers.size()<<endl;

	if (depth < 2)
	{
		eval_value = miniMaxPrune(alpha, beta);
	}
	else
	{
		cannons = get_cannon_pos();
		update_moves_count();
		eval_value = evaluate(sattack, snot_attack, townhall_attack);
		soldiers.clear();
		if(ourPlayer == 0)
		 color = 1;
		else
		 color = 0;
		if (color == 0)
	{
		for (short i = 0; i < 8; i++)
		{
			for (short j = 0; j < 8; j++)
				if (board_state[i][j] == 1)
					soldiers.push_back(make_pair(j, i));
		}
	}
	else
	{
		for (short i = 0; i < 8; i++)
		{
			for (short j = 0; j < 8; j++)
				if (board_state[i][j] == -1)
					soldiers.push_back(make_pair(j, i));
		}
	}
	cannons = get_cannon_pos();
		update_moves_count();
		eval_value = eval_value - 1.1 * evaluate(sattack, snot_attack, townhall_attack);
		color = ourPlayer;

	}

}

void state::Generate_childs()
{

}

bool state::search_sol(short x1, short y1){
    for(int i=0; i<soldiers.size(); i++){
        if(soldiers[i].first == x1 && soldiers[i].second == y1) return true;
    }
    return false;
}

float state::evaluate(short sattack, short snot_attack, short townhall_attack){
    float tot = 0, sum1 = 0;
    int n = 8, m = 8;
    float cann[] = {5, 3.2, 7.5, 7.5, 3};
    tot = tot + (sattack * 1.2) + (snot_attack * 1.0) + (townhall_attack * 100.0);

    for(int i=0; i < cannons.size(); i++){
        short x1 = (cannons[i].second).first;
        short y1 = (cannons[i].second).second;

        // vertical
        if(cannons[i].first == 0){
            if(y1-2 > 0 && !search_sol(x1, y1-2)){
                if(y1-4 >= 0 && (search_sol(x1, y1-3) || search_sol(x1, y1-4))){
                    tot += cann[0];
                    continue;
                }
                else if(y1-3 == 0 && search_sol(x1, y1-3)){
                    tot += cann[0];
                    continue;
                }
            }
            else if(y1+2 < m-1 && !search_sol(x1, y1-2)){
                if(y1+4 <= m-1 && (search_sol(x1, y1-3) || search_sol(x1, y1-4))){
                    tot += cann[0];
                    continue;
                }
                else if(y1+3 == m-1 && search_sol(x1, y1-3)){
                    tot += cann[0];
                    continue;
                }
            }
        }

        // horizontal
        else if(cannons[i].first == 1){
            if(x1-2 > 0 && !search_sol(x1, y1-2)){
                if(x1-4 >= 0 && (search_sol(x1 - 3, y1) || search_sol(x1 - 4, y1))){
                    tot += cann[1];
                    continue;
                }
                else if(x1-3 == 0 && search_sol(x1 - 3, y1)){
                    tot += cann[1];
                    continue;
                }
            }
            else if(x1+2 > n-1 && !search_sol(x1 - 2, y1)){
                if(x1+4 <= n-1 && (search_sol(x1 - 3, y1) || search_sol(x1 - 4, y1))){
                    tot += cann[1];
                    continue;
                }
                else if(x1+3 == n-1 && search_sol(x1 - 3, y1)){
                    tot += cann[1];
                    continue;
                }
            }
        }

        // right diagonal
        else if(cannons[i].first == 2){
            if((y1-2 > 0) && (x1-2 > 0) && !search_sol(x1-2, y1-2)){
                if((y1-4 >= 0) && (x1-4 >= 0) && (search_sol(x1-3, y1-3) || search_sol(x1-4, y1-4))){
                    tot += cann[2];
                    continue;
                }
                else if((y1-3 == 0) && (x1-3 == 0) && (search_sol(x1-3, y1-3))){
                    tot += cann[2];
                    continue;
                }
            }
            else if((y1+2 < m-1) && (x1+2 < n-1) && !search_sol(x1+2, y1+2)){
                if((y1+4 <= m-1) && (x1+4 <= n-1) && (search_sol(x1+3, y1+3) || search_sol(x1+4, y1+4))){
                    tot += cann[2];
                    continue;
                }
                else if((y1+3 == m-1) && (x1+3 == m-1) && search_sol(x1+3, y1+3)){
                    tot += cann[2];
                    continue;
                }
            }
        }

        // left diagonal
        else if(cannons[i].first == 3){
            if((y1-2 > 0) && (!search_sol(x1+2, y1-2))){
                if((y1-4 >= 0) && (x1+4 <= n-1) && (search_sol(x1+3, y1-3) || search_sol(x1+4, y1-4))){
                    tot += cann[3];
                    continue;
                }
                else if((y1-3 == 0) && (x1+3 == n-1) && (search_sol(x1+3, y1-3))){
                    tot += cann[3];
                    continue;
                }
            }
            else if((y1+2 < m-1) && (x1-2 > 0) && (!search_sol(x1-2, y1+2))){
                if((y1+4 <= m-1) && (x1-4 >=0) && (search_sol(x1-3, y1+3) || search_sol(x1-4, y1+4))){
                    tot += cann[3];
                    continue;
                }
                else if((y1+3 == m-1) && search_sol(x1-3, y1+3)){
                    tot += cann[3];
                    continue;
                }
            }
        }
        tot += cann[4];
    }

    for(int i=0; i<soldiers.size(); i++){
        sum1 += soldiers[i].second;
    }
    tot = tot - (sum1/soldiers.size());
    return tot;
}

void state::print_move()
{
	if (get<0>(fro_move) == 0)
	{
		cout << "S " << get<1>(fro_move).first << " " << get<1>(fro_move).second << " M " << get<2>(fro_move).first << " " << get<2>(fro_move).second << endl;
	}
	else
	{
		cout << "S " << get<1>(fro_move).first << " " << get<1>(fro_move).second << " B " << get<2>(fro_move).first << " " << get<2>(fro_move).second << endl;
	}
}

void state::update_moves_count()
{
//	cout<<"Here UPDATES"<<endl;
	for (auto c : cannons)
	{
		auto moves = get_cannon_moves(c);
		for (auto p : moves)
		{
			if (color == 0)
			{
				if (get<0>(p) == 1 && board_state[get<2>(p).second][get<2>(p).first] == (-2))
					townhall_attack+=50;
				else if (get<0>(p) == 1 && board_state[get<2>(p).second][get<2>(p).first] == (-1))
					cattack+=20;
				else if (get<0>(p) == 0)
					cnot_attack++;

			}
			else
			{
				if (get<0>(p) == 1 && board_state[get<2>(p).second][get<2>(p).first] == (2))
					townhall_attack+=50;
				else if (get<0>(p) == 1 && board_state[get<2>(p).second][get<2>(p).first] == (1))
					cattack+=20;
				else if (get<0>(p) == 0)
					cnot_attack++;
			}

		}
	}

	for (auto c : soldiers)
	{
		auto moves = get_soldier_moves(c);
		for (auto p : moves)
		{

			if (color == 0)
			{
				if (board_state[p.second][p.first] == (-2))
					townhall_attack+=50;
				else if (board_state[p.second][p.first] == (-1))
					sattack+=20;
				else
					snot_attack++;

			}
			else
			{
				if (board_state[p.second][p.first] == (2))
					townhall_attack+=50;
				else if (board_state[p.second][p.first] == (1))
					sattack+=20;
				else
					snot_attack++;
			}

		}
	}
}

vector<pair<short, short>> state::get_soldier_moves(pair<short,short> pos)
{
	vector<pair<short, short>> moves;
	//cout<<"Soldier Moves "<<color<<" "<<ourPlayer<<" "<<depth<<" "<<this->moves<<endl;
	//cout<<"Pos "<<pos.first << " "<<pos.second<<endl;
	//print_board();
	if (color == 0)
	{
		if (pos.second > 0)
		{
            if(board_state[pos.second - 1 ][pos.first] <= 0)
			 moves.push_back(make_pair(pos.first, pos.second - 1));
			if (pos.first > 0)
			{
				if(board_state[pos.second - 1 ][pos.first -1] <= 0)
				 moves.push_back(make_pair(pos.first - 1, pos.second - 1));
				if (board_state[pos.second][pos.first - 1] < 0)
					moves.push_back(make_pair(pos.first - 1, pos.second));

			}
			if (pos.first < 7)
			{
				if(board_state[pos.second - 1 ][pos.first + 1] <= 0)
					moves.push_back(make_pair(pos.first + 1, pos.second - 1));
				if (board_state[pos.second][pos.first + 1] < 0)
					moves.push_back(make_pair(pos.first + 1, pos.second));
			}
		}
        //cout<<"adj "<<endl;
		if (adjacent(pos))
		{
			if (pos.second + 2 < 8)
			{
				if(board_state[pos.second + 2 ][pos.first] <= 0)
				   moves.push_back(make_pair(pos.first, pos.second + 2));
				if ((pos.first - 2) >= 0 && board_state[pos.second + 2 ][pos.first - 2 ] <= 0)
					moves.push_back(make_pair(pos.first - 2, pos.second + 2));
				if ((pos.first + 2) < 8 && board_state[pos.second + 2 ][pos.first - 2] <= 0)
					moves.push_back(make_pair(pos.first + 2, pos.second + 2));
			}
		}



	}
	else
	{
		if (pos.second < 8)
		{
			if(board_state[pos.second + 1][pos.first] >= 0)
			 moves.push_back(make_pair(pos.first, pos.second + 1));
			if (pos.first > 0)
			{
				if(board_state[pos.second + 1][pos.first -1] >= 0)
				 moves.push_back(make_pair(pos.first - 1, pos.second + 1));
				if (board_state[pos.second][pos.first - 1] > 0)
					moves.push_back(make_pair(pos.first - 1, pos.second));

			}
			if (pos.first < 7)
			{
				if(board_state[pos.second + 1][pos.first + 1] >= 0)
				 moves.push_back(make_pair(pos.first + 1, pos.second + 1));
				if (board_state[pos.second][pos.first+1] > 0)
					moves.push_back(make_pair(pos.first + 1, pos.second));
			}
		}

		if (adjacent(pos))
		{
			if ((pos.second - 2) >= 0)
			{
				if(board_state[pos.second - 2][pos.first] >= 0)
				 moves.push_back(make_pair(pos.first, pos.second - 2));
				if ((pos.first - 2) >= 0 && board_state[pos.second - 2][pos.first - 2] >= 0)
					moves.push_back(make_pair(pos.first - 2, pos.second - 2));
				if ((pos.first + 2) < 8 && board_state[pos.second - 2][pos.first + 2] >= 0)
					moves.push_back(make_pair(pos.first + 2, pos.second - 2));
			}
		}
	}
	return moves;
}

bool state::adjacent(pair<short, short> pos)
{

	if (pos.second > 0)
	{
		if (board_state[pos.second - 1][pos.first] * board_state[pos.second][pos.first] < 0)
			return true;
		if (pos.first > 0)
		{
			if (board_state[pos.second - 1][pos.first - 1] * board_state[pos.second][pos.first] < 0)
				return true;
			if (board_state[pos.second][pos.first -1] * board_state[pos.second][pos.first] < 0)
				return true;
		}
		if (pos.first < 7)
		{
			if (board_state[pos.second -1][pos.first + 1] * board_state[pos.second][pos.first] < 0)
				return true;
			if (board_state[pos.second][pos.first + 1] * board_state[pos.second][pos.first] < 0)
				return true;
		}
	}
	if (pos.second < 7)
	{
		if ((board_state[pos.second +1][pos.first] * board_state[pos.second][pos.first]) < 0)
			return true;
		if (pos.first > 0)
		{
			if ((board_state[pos.second +1][pos.first - 1] * board_state[pos.second][pos.first]) < 0)
				return true;

		}
		if (pos.first < 7)
		{
			if ((board_state[pos.second + 1][pos.first+ 1] * board_state[pos.second][pos.first]) < 0)
				return true;

		}
	}
	return false;
}

vector<pair<short, pair<short, short>>> state::get_cannon_pos()
{
	vector<pair<short, pair<short, short>>> cannons;
	if (color == 0)
	{
		for (auto c : soldiers)
		{
			if (c.second > 0 && c.second < 7 && (board_state[c.second + 1][c.first] == 1) && (board_state[c.second -1][c.first] == 1))
				cannons.push_back(make_pair(0, c));
			if (c.first > 0 && c.first < 7 && (board_state[c.second][c.first+1] == 1) && (board_state[c.second][c.first-1] == 1))
				cannons.push_back(make_pair(1, c));
			if ((c.first > 0) && (c.second > 0) && (c.first < 7) && (c.second < 7) && (board_state[c.second - 1][c.first + 1] ==1) && (board_state[c.second + 1][c.first- 1] == 1))
				cannons.push_back(make_pair(2, c));
			if ((c.first > 0) && (c.second > 0) && (c.first < 7) && (c.second < 7) && (board_state[c.second + 1][c.first + 1] == 1) && (board_state[c.second - 1][c.first - 1] == 1))
				cannons.push_back(make_pair(3, c));

		}
	}
	else
	{
		for (auto c : soldiers)
		{
			if (c.second > 0 && c.second < 7 && (board_state[c.second + 1][c.first] == -1) && (board_state[c.second -1][c.first] == -1))
				cannons.push_back(make_pair(0, c));
			if (c.first > 0 && c.first < 7 && (board_state[c.second][c.first+1] == -1) && (board_state[c.second][c.first-1] == -1))
				cannons.push_back(make_pair(1, c));
			if ((c.first > 0) && (c.second > 0) && (c.first < 7) && (c.second < 7) && (board_state[c.second - 1][c.first + 1] == -1) && (board_state[c.second + 1][c.first- 1] == -1))
				cannons.push_back(make_pair(2, c));
			if ((c.first > 0) && (c.second > 0) && (c.first < 7) && (c.second < 7) && (board_state[c.second + 1][c.first + 1] == -1) && (board_state[c.second - 1][c.first - 1] == -1))
				cannons.push_back(make_pair(3, c));

		}

	}


	return cannons;
}

float state::miniMaxPrune(float alpha, float beta)
{

	cannons = get_cannon_pos();
	//cout<<cannons.size()<<" CANNO SIZE"<<endl;
	float val;
	if (color == ourPlayer)
		val = -100000;
	else
		val = FLT_MAX;
	for (auto c : cannons)
	{
		auto moves = get_cannon_moves(c);
		//cout<<depth<<" cmove "<<moves.size()<<endl;
		for (auto p : moves)
		{
			child_states.push_back(get_next_state(p,alpha,beta));
			if (color == ourPlayer)
			{
				if (val < child_states.back()->eval_value)
				{
					fro_move = p;
				}
				val = max(val, child_states.back()->eval_value);
				if (val >= beta || (alpha>=beta))
					return val;
				alpha = max(alpha, val);
			}
			else
			{
				if (val > child_states.back()->eval_value)
				{
					fro_move = p;
				}

				val = min(val, child_states.back()->eval_value);
				if (val <= alpha || (alpha>=beta))
					return val;
				beta = min(beta, val);
			}

		}
	}

	for (auto c : soldiers)
	{
		auto moves = get_soldier_moves(c);
		//cout<<moves.size()<<endl;
		for (auto p : moves)
		{
			child_states.push_back(get_next_state(make_tuple(0,c,p), alpha, beta));
			if (color == ourPlayer)
			{
				if (val < child_states.back()->eval_value)
				{
					fro_move = make_tuple(0,c,p);
				}

				val = max(val, child_states.back()->eval_value);
				if (val >= beta || (alpha>=beta))
					return val;
				alpha = max(alpha, val);
			}
			else
			{
				if (val > child_states.back()->eval_value)
				{
					fro_move = make_tuple(0, c, p);
				}
				val = min(val, child_states.back()->eval_value);
				if (val <= alpha || (alpha>=beta))
					return val;
				beta = min(beta, val);
			}

		}
	}
	return val;
}

vector<tuple<short, pair<short, short>, pair<short, short>>> state::get_cannon_moves(pair<short,pair<short, short>> cannon)
{
	vector<tuple<short, pair<short, short>, pair<short, short>>> moves;
	pair<short, short> pos = cannon.second;
	if (color == 0)
	{
		if (cannon.first == 0)
		{
			if (pos.second > 1 && board_state[pos.second - 2][pos.first] == 0)
				moves.push_back(make_tuple(0, make_pair(pos.first, pos.second + 1), make_pair(pos.first, pos.second - 2)));
			if (pos.second < 6 && board_state[pos.second+2][pos.first] == 0)
				moves.push_back(make_tuple(0, make_pair(pos.first, pos.second - 1), make_pair(pos.first, pos.second + 2)));

			if (pos.second > 2 && board_state[pos.second - 3][pos.first] < 0)
			{
				moves.push_back(make_tuple(1,pos,make_pair(pos.first,pos.second - 3)));
			}
			if (pos.second < 5 && board_state[pos.second + 3][pos.first] < 0)
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first, pos.second + 3)));
			}
			if (pos.second > 3 && board_state[pos.second - 4][pos.first] < 0)
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first, pos.second - 4)));
			}
			if (pos.second < 4 && board_state[pos.second + 4][pos.first] < 0)
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first, pos.second + 4)));
			}


		}
		else if (cannon.first == 1)
		{
			if (pos.first > 2 && board_state[pos.second][pos.first -3] < 0)
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first -3, pos.second)));
			}
			if (pos.first < 5 && board_state[pos.second][pos.first+3] < 0)
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first + 3, pos.second)));
			}
			if (pos.first > 3 && board_state[pos.second][pos.first-4] < 0)
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first - 4, pos.second)));
			}
			if (pos.first < 4 && board_state[pos.second][pos.first+4] < 0)
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first + 4, pos.second)));
			}

			if (pos.first > 1 && board_state[pos.second][pos.first-2] == 0)
				moves.push_back(make_tuple(0, make_pair(pos.first + 1, pos.second), make_pair(pos.first - 2 , pos.second)));
			if (pos.first < 6 && board_state[pos.second][pos.first+2] == 0)
				moves.push_back(make_tuple(0, make_pair(pos.first - 1, pos.second), make_pair(pos.first + 2, pos.second)));
		}
		else if (cannon.first == 2)
		{
			if ((pos.second - 3 ) >= 0 && ((pos.first + 3) < 8) && (board_state[pos.second -3][pos.first + 3] < 0))
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first + 3, pos.second - 3)));
			}
			if ((pos.second + 3) <8 && ((pos.first - 3) >=0) && (board_state[pos.second + 3][pos.first - 3] < 0))
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first -3, pos.second + 3)));
			}
			if ((pos.second - 4) >= 0 && ((pos.first + 4) < 8) && (board_state[pos.second - 4][pos.first + 4] < 0))
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first + 4, pos.second - 4)));
			}
			if ((pos.second + 4) < 8 && ((pos.first - 4) >= 0) && (board_state[pos.second + 4][pos.first - 4] < 0))
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first - 4, pos.second + 4)));
			}

			if (((pos.second - 2) >= 0) && ((pos.first +2) <8 ) && board_state[pos.second - 2][pos.first + 2] == 0)
				moves.push_back(make_tuple(0, make_pair(pos.first - 1, pos.second + 1), make_pair(pos.first + 2, pos.second - 2)));
			if (((pos.second + 2) < 8) && ((pos.first - 2) >= 2) && board_state[pos.second +1][pos.first -2] == 0)
				moves.push_back(make_tuple(0, make_pair(pos.first + 1, pos.second - 1), make_pair(pos.first - 2, pos.second + 2)));
		}
		else if (cannon.first == 3)
		{
			if ((pos.second - 3) >= 0 && ((pos.first - 3) >= 0) && (board_state[pos.second- 3][pos.first - 3] < 0))
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first - 3, pos.second - 3)));
			}
			if ((pos.second + 3) < 8 && ((pos.first + 3) < 8) && (board_state[pos.second + 3][pos.first + 3] < 0))
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first + 3, pos.second + 3)));
			}
			if ((pos.second - 4) >= 0 && ((pos.first - 4) >= 0) && (board_state[pos.second - 4][pos.first - 4] < 0))
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first - 4, pos.second - 4)));
			}
			if ((pos.second + 4) < 8 && ((pos.first + 4) < 8) && (board_state[pos.second + 4][pos.first + 4] < 0))
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first + 4, pos.second + 4)));
			}

			if (((pos.second - 2) >= 0) && ((pos.first - 2) >= 0) && board_state[pos.second - 2][pos.first - 2] == 0)
				moves.push_back(make_tuple(0, make_pair(pos.first + 1, pos.second + 1), make_pair(pos.first - 2, pos.second - 2)));
			if (((pos.second + 2) < 8) && ((pos.first + 2) < 8) && board_state[pos.second + 2][pos.first + 2] == 0)
				moves.push_back(make_tuple(0, make_pair(pos.first - 1, pos.second - 1), make_pair(pos.first + 2, pos.second + 2)));
		}
	}
	else
	{
		if (cannon.first == 0)
		{
			if (pos.second > 1 && board_state[pos.second - 2][pos.first] == 0)
				moves.push_back(make_tuple(0, make_pair(pos.first, pos.second + 1), make_pair(pos.first, pos.second - 2)));
			if (pos.second < 6 && board_state[pos.second+2][pos.first] == 0)
				moves.push_back(make_tuple(0, make_pair(pos.first, pos.second - 1), make_pair(pos.first, pos.second + 2)));

			if (pos.second > 2 && board_state[pos.second - 3][pos.first] > 0)
			{
				moves.push_back(make_tuple(1,pos,make_pair(pos.first,pos.second - 3)));
			}
			if (pos.second < 5 && board_state[pos.second + 3][pos.first] > 0)
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first, pos.second + 3)));
			}
			if (pos.second > 3 && board_state[pos.second - 4][pos.first] > 0)
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first, pos.second - 4)));
			}
			if (pos.second < 4 && board_state[pos.second + 4][pos.first] > 0)
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first, pos.second + 4)));
			}


		}
		else if (cannon.first == 1)
		{
			if (pos.first > 2 && board_state[pos.second][pos.first -3] > 0)
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first -3, pos.second)));
			}
			if (pos.first < 5 && board_state[pos.second][pos.first+3] > 0)
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first + 3, pos.second)));
			}
			if (pos.first > 3 && board_state[pos.second][pos.first-4] > 0)
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first - 4, pos.second)));
			}
			if (pos.first < 4 && board_state[pos.second][pos.first+4] > 0)
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first + 4, pos.second)));
			}

			if (pos.first > 1 && board_state[pos.second][pos.first-2] == 0)
				moves.push_back(make_tuple(0, make_pair(pos.first + 1, pos.second), make_pair(pos.first - 2 , pos.second)));
			if (pos.first < 6 && board_state[pos.second][pos.first+2] == 0)
				moves.push_back(make_tuple(0, make_pair(pos.first - 1, pos.second), make_pair(pos.first + 2, pos.second)));
		}
		else if (cannon.first == 2)
		{
			if ((pos.second - 3 ) >= 0 && ((pos.first + 3) < 8) && (board_state[pos.second -3][pos.first + 3] > 0))
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first + 3, pos.second - 3)));
			}
			if ((pos.second + 3) <8 && ((pos.first - 3) >=0) && (board_state[pos.second + 3][pos.first - 3] > 0))
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first -3, pos.second + 3)));
			}
			if ((pos.second - 4) >= 0 && ((pos.first + 4) < 8) && (board_state[pos.second - 4][pos.first + 4] > 0))
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first + 4, pos.second - 4)));
			}
			if ((pos.second + 4) < 8 && ((pos.first - 4) >= 0) && (board_state[pos.second + 4][pos.first - 4] > 0))
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first - 4, pos.second + 4)));
			}

			if (((pos.second - 2) >= 0) && ((pos.first +2) <8 ) && board_state[pos.second - 2][pos.first + 2] == 0)
				moves.push_back(make_tuple(0, make_pair(pos.first - 1, pos.second + 1), make_pair(pos.first + 2, pos.second - 2)));
			if (((pos.second + 2) < 8) && ((pos.first - 2) >= 2) && board_state[pos.second +1][pos.first -2] == 0)
				moves.push_back(make_tuple(0, make_pair(pos.first + 1, pos.second - 1), make_pair(pos.first - 2, pos.second + 2)));
		}
		else if (cannon.first == 3)
		{
			if ((pos.second - 3) >= 0 && ((pos.first - 3) >= 0) && (board_state[pos.second- 3][pos.first - 3] > 0))
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first - 3, pos.second - 3)));
			}
			if ((pos.second + 3) < 8 && ((pos.first + 3) < 8) && (board_state[pos.second + 3][pos.first + 3] > 0))
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first + 3, pos.second + 3)));
			}
			if ((pos.second - 4) >= 0 && ((pos.first - 4) >= 0) && (board_state[pos.second - 4][pos.first - 4] > 0))
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first - 4, pos.second - 4)));
			}
			if ((pos.second + 4) < 8 && ((pos.first + 4) < 8) && (board_state[pos.second + 4][pos.first + 4] > 0))
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first + 4, pos.second + 4)));
			}

			if (((pos.second - 2) >= 0) && ((pos.first - 2) >= 0) && board_state[pos.second - 2][pos.first - 2] == 0)
				moves.push_back(make_tuple(0, make_pair(pos.first + 1, pos.second + 1), make_pair(pos.first - 2, pos.second - 2)));
			if (((pos.second + 2) < 8) && ((pos.first + 2) < 8) && board_state[pos.second + 2][pos.first + 2] == 0)
				moves.push_back(make_tuple(0, make_pair(pos.first - 1, pos.second - 1), make_pair(pos.first + 2, pos.second + 2)));
		}
	}

	return moves;
}


/*float state::evaluate()
{
	if(color == ourPlayer)
	 return (cattack+cnot_attack+sattack+snot_attack+townhall_attack);
	return (cattack+cnot_attack+sattack+snot_attack+townhall_attack);
}
*/
float state::volaitlity(pair<short, short> pos)
{
	return 0;
}

short state::get_val()
{
	return eval_value;
}

state* state::get_next_state()
{
	short ncol;
	if (color == 1)
		ncol = 0;
	else
		ncol = 1;
    //cout<<child_states.size()<<"child state size"<<endl;
	for( auto c: child_states)
	 if(c->back_move == this->fro_move)
	  return c;


	return new state();
}

state* state::get_next_state(tuple<short, pair<short, short>, pair<short, short>> move,float alpha,float beta)
{
	short ncol;

	if (color == 1)
		ncol = 0;
	else
		ncol = 1;
    //cout<<"hit "<<get<0>(move)<<" "<<get<1>(move).first<<get<1>(move).second<<" "<<get<2>(move).first<<get<2>(move).second<<endl;
	state * new_state = new state(board_state, move, depth + 1, ncol,ourPlayer,alpha,beta,moves + 1);
	//cout<<"GET NEXT STATE"<<endl;
	//new_state->print_board();
	//cout<<endl<<depth<<" "<<alpha<<" "<<beta<<" "<<moves<<endl;
	return new_state;
}

vector<vector<short>> state::get_next_board(tuple<short, pair<short, short>, pair<short, short>> move)
{
	vector<vector<short>> new_board = this->board_state;

	if (get<0>(move) == 0)
	{
		new_board[get<2>(move).second][get<2>(move).first] = new_board[get<1>(move).second][get<1>(move).first];
		new_board[get<1>(move).second][get<1>(move).first] = 0;
		return new_board;

	}
	else if (get<0>(move) == 1)
	{
		new_board[get<2>(move).second][get<2>(move).first] = 0;

	}

	return new_board;
}

float state::get_pos_voltil(pair<short, pair<short, short>>)
{
	return 0.0f;
}

void state::print_board()
{
	for (short i = 0; i < 8; i++)
	{
		for (short j = 0; j < 8; j++)
			cout << board_state[i][j] << " ";
		cout << endl;
	}

}



int main()
{
	short ourName,time_lim,n,m;
	string input_str;
	getline(cin, input_str);
	stringstream istr(input_str);
	state *game;
	istr >> ourName;
	ourName -= 1;
	istr >> n;
	istr >> m;
	istr >> time_lim;
	vector<vector<short>> board_state = { {-2,-1,-2,-1,-2,-1,-2,-1},
	{0,-1,0,-1,0,-1,0,-1},
	{0,-1,0,-1,0,-1,0,-1},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{1,0,1,0,1,0,1,0},
	{1,0,1,0,1,0,1,0},
	{1,2,1,2,1,2,1,2},
	};

	string move;
	char fi, mtype;
	int x1, y1, x2, y2;
	if (ourName == 1)
	{
		getline(cin, move);
		stringstream itr(move);
		itr >> fi >> x1 >> y1 >> mtype >> x2 >> y2;
		tuple<short, pair<short, short>, pair<short, short>> nex_move;
		if (mtype == 'M')
			nex_move = make_tuple(0, make_pair(x1, y1), make_pair(x2, y2));
		else
			nex_move = make_tuple(1, make_pair(x1, y1), make_pair(x2, y2));
		//cout<<"A " << (get<0>(nex_move))<<endl;
		//cout<<(get<1>(nex_move).second)<<endl;
		//cout << " 0" << endl;
		game = new state(board_state, nex_move, 0, 1, 1, FLT_MIN, FLT_MAX, 1);
		//cout << " 1" << endl;
		game->print_move();
		//game->print_board();
		game = game->get_next_state();
		//game->print_board();
	}
	else
	{
		game = new state(board_state,make_tuple(0,make_pair(0,0),make_pair(0,0)), 0, 0, 0, FLT_MIN, FLT_MAX, 1);
		game->print_move();
		game = game->get_next_state();
	}

	while (true)
	{
		getline(cin, move);
		stringstream itr(move);
		itr >> fi >> x1 >> y1 >> mtype >> x2 >> y2;
		tuple<short, pair<short, short>, pair<short, short>> nex_move;
		if (mtype == 'M')
			nex_move = make_tuple(0, make_pair(x1, y1), make_pair(x2, y2));
		else
			nex_move = make_tuple(1, make_pair(x1, y1), make_pair(x2, y2));
		game = new state(game->board_state, nex_move, 0, ourName, ourName, FLT_MIN, FLT_MAX, 1);
//		game->print_board();
		game->print_move();
		game = game->get_next_state();
//		game->print_board();

	}
	return 0;

}
