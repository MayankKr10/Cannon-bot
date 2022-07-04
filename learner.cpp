#include "iostream"
#include "vector"
#include "map"
#include "unordered_map"
#include "algorithm"
#include "thread"
#include "utility"
#include "limits"
#include "string"
#include "fstream"
#include "sstream"
#include "float.h"
#include "stdlib.h"
using namespace std;
ofstream outfile("v.txt");
//return (10*cattack+cnot_attack+sattack+snot_attack+5*townhall_attack+ 15000*townhalls + 100*soldiers.size() + 20*cannons.size());
vector<float> our_weights = {6.13647, -59.4343 ,11.6703 ,6.65951 ,47.861, 15000.3, 101.372 ,20.306, 24.6096 ,1};
//9.22843 1.65918 4.17032 1.03211 26.5467 15000.8 100.102 20.0077 3.93233 1 
//11.0153 -72.4358 5.3693 4.98997 38.5622 15000.3 101.038 20.3571 13.9541 1
//10.6622 -68.8715 8.85293 5.59539 38.6011 15000.4 101.182 20.3368 17.1841 1 
//9.91335 -54.4447 7.99278 4.64748 42.1597 15000.4 100.952 20.2853 16.0232 1
//6.13647 -59.4343 11.6703 6.65951 47.861 15000.3 101.372 20.306 24.6096 1
vector<float> our_vals = {10,1,1,1,5,15000,100,20,0,0};
vector<float> other_weights = {10.5397 ,-19.219, -3.16168 ,-1.66158, -6.1397 ,14998.5 ,98.7525 ,19.953, -17.7831, 1 };
//10.2308 0.182652 0.343772 0.790268 5.01089 15000.3 99.9983 20.0266 -0.304715 1 
//9.73161 2.63387 0.12483 -1.0505 4.95371 14999.4 99.054 19.8955 -6.98189 1 
//9.26967 -3.85131 -2.16359 -1.11784 5.39714 14999.2 98.9647 19.8663 -10.1328 1 
//9.43997 -18.0932 -1.8388 -0.597761 0.838367 14999.1 99.1793 19.9156 -7.22623 1 
//10.5397 -19.219 -3.16168 -1.66158 -6.1397 14998.5 98.7525 19.9533 -17.7831 1 
vector<float> other_vals = {10,1,1,1,5,15000,100,20,0,0};
class state
{
public:

	vector<vector<short>> board_state;
	short depth,color,ourPlayer,moves,n,m;
	short townhalls;
	tuple<short, pair<short,short>, pair<short,short>> back_move;
	tuple<short, pair<short, short>, pair<short, short>> fro_move;
	vector<pair<short, short>> soldiers;
	vector<pair<short,pair<short, short>>> cannons;
	float eval_value;
	vector<state*> child_states;
	short sattack, snot_attack,cnot_attack,cattack,townhall_attack;

	state(short a, short b);
	state(vector<vector<short>> board, tuple<short, pair<short, short>, pair<short, short>> move, short dep, short col,short oPlayer,float alpha,float beta,short movestill,short n,short m);
	void print_move();
	void print_this_move(tuple<short, pair<short, short>, pair<short, short>> move_s);
	void update_moves_count();
	vector<pair<short, short>> get_soldier_moves(pair<short,short>);
	vector<tuple<short, pair<short, short>, pair<short, short>>> get_cannon_moves(pair<short,pair<short, short>>);
	float evaluate();
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
	bool check_State();
	float position_incentives();
	void weights_update();
	void print_weights();
	float this_eval();	
	void clear_childs();
	~state();
};


state::~state()
{
	clear_childs();
	child_states.clear();
	soldiers.clear();
}

void state::clear_childs()
{
	for(auto p:child_states)
	{
		delete(p);
	}
}

state::state(short a,short b)
{
	depth = eval_value = color = 0;
	ourPlayer = moves = 0;
	sattack = cattack = 0;
	townhalls = 4;
	n = a;
	m = b;
	fro_move = make_tuple(0,make_pair(4,7),make_pair(5,6));
	if(n==8 && m==10)
		fro_move = make_tuple(0,make_pair(4,9),make_pair(5,8));
	if(n==10 && m==10)
		fro_move = make_tuple(0,make_pair(4,9),make_pair(5,8));
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
	if(n==10 && m==8)
	{
		board_state = { {-2,-1,-2,-1,-2,-1,-2,-1},
		{0,-1,0,-1,0,-1,0,-1},
		{0,-1,0,-1,0,-1,0,-1},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{1,0,1,0,1,0,1,0},
		{1,0,1,0,1,0,1,0},
		{1,2,1,2,1,2,1,2},
		};
	}
	if(n==10 && m==10)
	{
		board_state = {{-2,-1,-2,-1,-2,-1,-2,-1,-2,-1},
		{0,-1,0,-1,0,-1,0,-1,0,-1},
		{0,-1,0,-1,0,-1,0,-1,0,-1},
		{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},
		{1,0,1,0,1,0,1,0,1,0},
		{1,0,1,0,1,0,1,0,1,0},
		{1,2,1,2,1,2,1,2,1,2},
		};
	}


}

state::state(vector<vector<short>> board, tuple<short, pair<short, short>, pair<short, short>> move,short dep,short col,short oPlayer,float alpha,float beta,short movestill,short n,short m)
{
	back_move = move;
	snot_attack = cnot_attack = 0;
	ourPlayer = oPlayer;
	board_state = board;
	moves = movestill;
	fro_move = make_tuple(0,make_pair(0,0),make_pair(0,0));
	//cout<<"Hello "<<endl;
	this->n = n;
	this->m = m;
	if (get<0>(move) == 0)
	{

		board_state[get<2>(move).second][get<2>(move).first] = board_state[get<1>(move).second][get<1>(move).first];
		board_state[get<1>(move).second][get<1>(move).first] = 0;

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
		//cout<<"n "<<this->n<<" m "<<this->m<<endl;
		for (short i = 0; i < n; i++)
		{
			for (short j = 0; j < m; j++)
				{
					if (board_state[i][j] == 1)
					 soldiers.push_back(make_pair(j, i));
			        else if(board_state[i][j] == 2)
				     townhalls++;
				}
		}
	}
	else
	{
		for (short i = n-1; i >= 0; i--)
		{
			for (short j = m-1; j >= 0; j--)
				{
					if (board_state[i][j] == -1)
					 soldiers.push_back(make_pair(j, i));
			        else if(board_state[i][j] == -2)
				     townhalls++;
				}
		}
	}
	/*
	if(check_State())
	{
	 if(ourPlayer == color)
	    eval_value = -1000000;
	 else
	    eval_value = 1000000;
	 if(depth == 0)
			exit(0);
	 return;
	}
*/	cannons = get_cannon_pos();
	update_moves_count();
	if (depth < 4 && !check_State() && !(cattack==0 && cnot_attack==0 && sattack==0 && snot_attack==0 && townhall_attack==0))
	{
		//cout<<"Minimax "<<endl;
		eval_value = miniMaxPrune(alpha, beta);
		weights_update();
		print_weights();
		//cout<<"Minimax 1"<<endl;
	}
	else
	{
		
		eval_value = 0;
		float n_eval = 0;
		if(ourPlayer==color)
		{
			our_vals[0] = cattack;	our_vals[1] = cnot_attack;
			our_vals[2] = sattack;	our_vals[3] = snot_attack;
			our_vals[4] = townhall_attack;	our_vals[5] = townhalls;
			our_vals[6] = soldiers.size(); our_vals[7] = cannons.size();
			our_vals[8] = position_incentives();
			our_vals[9] = evaluate();
		}
		else
		{
			other_vals[0] = cattack;	other_vals[1] = cnot_attack;
			other_vals[2] = sattack;	other_vals[3] = snot_attack;
			other_vals[4] = townhall_attack;	other_vals[5] = townhalls;
			other_vals[6] = soldiers.size(); other_vals[7] = cannons.size();
			other_vals[8] = position_incentives();
			other_vals[9] = evaluate();
		}

		if(soldiers.size() == 0 || (townhalls<3))
		 {
			 if(ourPlayer == color)
		       n_eval = -1000000/(depth+1);
		     else
		       n_eval = 1000000/(depth+1);	 
			eval_value = n_eval;
			if(depth==0)
				exit(0);
			return ;
		}
		if(cattack==0 && cnot_attack==0 && sattack==0 && snot_attack==0 && townhall_attack==0)
		 {
			 if(ourPlayer == color)
		       n_eval = -100000/(depth+1);
		     else
		       n_eval = 100000/(depth+1);
			eval_value = n_eval;
			if(depth==0)
				exit(0);
			return ;
		}
		
		
		if(color == ourPlayer)
		  n_eval += ( our_weights[8]*our_vals[8]+ our_vals[9]*our_weights[9]);
		 else
		  n_eval -= (other_weights[8]*other_vals[8] + other_vals[9]*other_weights[9]);
		soldiers.clear();
		if(color == 0)
		  color = 1;
		else
		  color = 0;
        short oc = townhalls;
        townhalls =0;
		if (color == 0)
		{
			for (short i = 0; i < n; i++)
			{
				for (short j = 0; j < m; j++)
				{
					if (board_state[i][j] == 1)
					 soldiers.push_back(make_pair(j, i));
			        else if(board_state[i][j] == 2)
				     townhalls++;
				}
			}
		}
		else
		{
			for (short i = n-1; i >= 0; i--)
			{
				for (short j = m-1; j >= 0; j--)
				{
					if (board_state[i][j] == -1)
					 soldiers.push_back(make_pair(j, i));
			        else if(board_state[i][j] == -2)
				     townhalls++;
				}
			}
		}
		if(soldiers.size() == 0 || (townhalls<3) )
		 {
			 if(ourPlayer == color)
		       n_eval = -1000000/(depth+1);
		     else
		       n_eval = 1000000/(depth+1);
			eval_value = n_eval;
			if(depth==0)
				exit(0);
			return ;
		 }
		 cannons = get_cannon_pos();
		 update_moves_count();
		 if(ourPlayer==color)
		{
			our_vals[0] = cattack;	our_vals[1] = cnot_attack;
			our_vals[2] = sattack;	our_vals[3] = snot_attack;
			our_vals[4] = townhall_attack;	our_vals[5] = townhalls;
			our_vals[6] = soldiers.size(); our_vals[7] = cannons.size();
			our_vals[8] = position_incentives();
			our_vals[9] = evaluate();
		}
		else
		{
			other_vals[0] = cattack;	other_vals[1] = cnot_attack;
			other_vals[2] = sattack;	other_vals[3] = snot_attack;
			other_vals[4] = townhall_attack;	other_vals[5] = townhalls;
			other_vals[6] = soldiers.size(); other_vals[7] = cannons.size();
			other_vals[8] = position_incentives();
			other_vals[9] = evaluate();
		}
		 if(cattack==0 && cnot_attack==0 && sattack==0 && snot_attack==0 && townhall_attack==0)
		 {
			 if(ourPlayer == color)
		       n_eval = -100000/(depth+1);
		     else
		       n_eval = 100000/(depth+1);
			eval_value = n_eval;
			if(depth==0)
				exit(0);
			return ;
		 }

		if(color == ourPlayer)
		  n_eval += ( our_weights[8]*our_vals[8]+ our_vals[9]*our_weights[9]);
		 else
		  n_eval -= (other_weights[8]*other_vals[8] + other_vals[9]*other_weights[9]);
		  
		
		if(color == 0)
		  color = 1;
		else
		  color = 0;
        townhalls = oc;
		eval_value = n_eval;

	}
	

}


void state::print_weights()
{
	outfile<<"OUR : ";
	outfile<<eval_value<<" ";
	print_board();
	for(int i=0;i<10;i++)
		outfile<<our_weights[i]<<" ";
	outfile<<endl;
	for(int i=0;i<10;i++)
		outfile<<our_vals[i]<<" ";
	outfile<<endl;
	for(int i=0;i<10;i++)
		outfile<<other_weights[i]<<" ";
	outfile<<endl;
	for(int i=0;i<10;i++)
		outfile<<other_vals[i]<<" ";
	outfile<<endl;
}
float state::position_incentives()
{
	float res = 0;
	if(color==0)
	{
		for(auto p:soldiers)
		{
			res += (5*(n-p.second));
			if(p.second <= 2)
				res += 10 * (n-p.second);
		}
	}
	else
	{
		for(auto p:soldiers)
		{
			res += (5*p.second);
			if(p.second >= (n-3))
				res += 10*(p.second);

		}
	}
	
	return res;
}

bool state::check_State()
{
	int a,b,c,d;
	a=b=c=d=0;
	for(int i = 0 ;i<n;i++)
		for(int j=0;j<m;j++)
		{
			if(board_state[i][j]==-2)
				a++;
			else if(board_state[i][j]==-1)
				b++;
			else if(board_state[i][j]==1)
				c++;
			else if(board_state[i][j]==2)
				d++;
			
		}
	if( a<3 || d<3 || b==0 || c==0)
		return true;
	return false;
}

void state::weights_update()
{
	float diff = eval_value - this_eval();
	outfile<<" THIS_EVAL " <<eval_value - diff<<" "<<endl;
	for (int i = 0; i < our_weights.size()-1; i++)
	{
		our_weights[i] += (0.000001 * diff*(our_vals[i]/eval_value));
	}
	for (int i = 0; i < other_weights.size()-1; i++)
	{
		other_weights[i] -= (0.000001 * diff*(other_vals[i]/eval_value));
	}

	
}

float state::this_eval()
{
		float n_eval = 0;
		if(ourPlayer==color)
		{
			our_vals[0] = cattack;	our_vals[1] = cnot_attack;
			our_vals[2] = sattack;	our_vals[3] = snot_attack;
			our_vals[4] = townhall_attack;	our_vals[5] = townhalls;
			our_vals[6] = soldiers.size(); our_vals[7] = cannons.size();
			our_vals[8] = position_incentives();
			our_vals[9] = evaluate();
		}
		else
		{
			other_vals[0] = cattack;	other_vals[1] = cnot_attack;
			other_vals[2] = sattack;	other_vals[3] = snot_attack;
			other_vals[4] = townhall_attack;	other_vals[5] = townhalls;
			other_vals[6] = soldiers.size(); other_vals[7] = cannons.size();
			other_vals[8] = position_incentives();
			other_vals[9] = evaluate();
		}

		if(soldiers.size() == 0 || (townhalls<3))
		 {
			 if(ourPlayer == color)
		       n_eval = -1000000/(depth+1);
		     else
		       n_eval = 1000000/(depth+1);	 
			return n_eval;
		}
		if(cattack==0 && cnot_attack==0 && sattack==0 && snot_attack==0 && townhall_attack==0)
		 {
			 if(ourPlayer == color)
		       n_eval = -100000/(depth+1);
		     else
		       n_eval = 100000/(depth+1);
			return n_eval;
		}
		
		
		if(color == ourPlayer)
		  n_eval += ( our_weights[8]*our_vals[8]+ our_vals[9]*our_weights[9]);
		 else
		  n_eval -= (other_weights[8]*other_vals[8] + other_vals[9]*other_weights[9]);
		soldiers.clear();
		if(color == 0)
		  color = 1;
		else
		  color = 0;
        short oc = townhalls;
        townhalls =0;
		if (color == 0)
		{
			for (short i = 0; i < n; i++)
			{
				for (short j = 0; j < m; j++)
				{
					if (board_state[i][j] == 1)
					 soldiers.push_back(make_pair(j, i));
			        else if(board_state[i][j] == 2)
				     townhalls++;
				}
			}
		}
		else
		{
			for (short i = n-1; i >= 0; i--)
			{
				for (short j = m-1; j >= 0; j--)
				{
					if (board_state[i][j] == -1)
					 soldiers.push_back(make_pair(j, i));
			        else if(board_state[i][j] == -2)
				     townhalls++;
				}
			}
		}
		cannons = get_cannon_pos();
		update_moves_count();
		if(ourPlayer==color)
		{
			our_vals[0] = cattack;	our_vals[1] = cnot_attack;
			our_vals[2] = sattack;	our_vals[3] = snot_attack;
			our_vals[4] = townhall_attack;	our_vals[5] = townhalls;
			our_vals[6] = soldiers.size(); our_vals[7] = cannons.size();
			our_vals[8] = position_incentives();
			our_vals[9] = evaluate();
		}
		else
		{
			other_vals[0] = cattack;	other_vals[1] = cnot_attack;
			other_vals[2] = sattack;	other_vals[3] = snot_attack;
			other_vals[4] = townhall_attack;	other_vals[5] = townhalls;
			other_vals[6] = soldiers.size(); other_vals[7] = cannons.size();
			other_vals[8] = position_incentives();
			other_vals[9] = evaluate();
		}
		if(soldiers.size() == 0 || (townhalls<3) )
		 {
			 if(ourPlayer == color)
		       n_eval = -1000000/(depth+1);
		     else
		       n_eval = 1000000/(depth+1);
			return n_eval;
		 }
		 
		 if(cattack==0 && cnot_attack==0 && sattack==0 && snot_attack==0 && townhall_attack==0)
		 {
			 if(ourPlayer == color)
		       n_eval = -100000/(depth+1);
		     else
		       n_eval = 100000/(depth+1);
			return n_eval;
		 }

		if(color == ourPlayer)
		  n_eval += ( our_weights[8]*our_vals[8]+ our_vals[9]*our_weights[9]);
		 else
		  n_eval -= (other_weights[8]*other_vals[8] + other_vals[9]*other_weights[9]);
		
		if(color == 0)
		  color = 1;
		else
		  color = 0;
        townhalls = oc;
		return n_eval;
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
	for (auto c : cannons)
	{
		auto moves = get_cannon_moves(c);
		for (auto p : moves)
		{
			if (color == 0)
			{
				if (get<0>(p) == 1 && board_state[get<2>(p).second][get<2>(p).first] == (-2))
					townhall_attack+=500;
				else if (get<0>(p) == 1 && board_state[get<2>(p).second][get<2>(p).first] == (-1))
					cattack+=100;
				else if (get<0>(p) == 0)
					{
						cnot_attack+=5;
						auto r = get<2>(p);
						if(r.second -1 >= 0 && (board_state[r.second-1][r.first])==-1 )
							cnot_attack -= 300;
						if(r.second -1 >= 0 && (r.first-1 >= 0) && (board_state[r.second-1][r.first-1])==-1 )
							cnot_attack -= 300;
						if(r.first-1 >=0 && (board_state[r.second][r.first-1]==-1))
							cnot_attack-=300;
						if(r.first+1 <m && (board_state[r.second][r.first+1]==-1))
							cnot_attack-=300;						
						if(r.second -1 >= 0 && (r.first+1 < m) && (board_state[r.second-1][r.first+1])==-1 )
							cnot_attack -= 300;
					}


			}
			else
			{
				if (get<0>(p) == 1 && board_state[get<2>(p).second][get<2>(p).first] == (2))
					townhall_attack+=500;
				else if (get<0>(p) == 1 && board_state[get<2>(p).second][get<2>(p).first] == (1))
					cattack+=50;
				else if (get<0>(p) == 0)
					{
						cnot_attack+=5;
						auto r = get<2>(p);
						if(r.second +1 <n && (board_state[r.second+1][r.first])==1 )
							cnot_attack -= 30;
						if(r.second +1 <n && (r.first-1 >= 0) && (board_state[r.second+1][r.first-1])==1 )
							cnot_attack -= 30;
						if(r.first-1 >=0 && (board_state[r.second][r.first-1]==1))
							cnot_attack-=30;
						if(r.first+1 <m && (board_state[r.second][r.first+1]==1))
							cnot_attack-=30;						
						if(r.second -1 >= 0 && (r.first+1 < m) && (board_state[r.second-1][r.first+1])==1 )
							cnot_attack -= 30;
					}

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
					townhall_attack+=500;
				else if (board_state[p.second][p.first] == (-1))
					{
						sattack+=20;
						if(p.second -1 >= 0 && (board_state[p.second-1][p.first])==-1 )
							sattack -= 15;
						if(p.second -1 >= 0 && (p.first-1 >= 0) && (board_state[p.second-1][p.first-1])==-1 )
							sattack -= 15;
						if(p.first-1 >=0 && (board_state[p.second][p.first-1]==-1))
							sattack-= 15;
						if(p.first+1 <m && (board_state[p.second][p.first+1]==-1))
							sattack-= 15;						
						if(p.second -1 >= 0 && (p.first+1 < m) && (board_state[p.second-1][p.first+1])==-1 )
							sattack -= 15;
					}
				else
					snot_attack++;

			}
			else
			{
				if (board_state[p.second][p.first] == (2))
					townhall_attack+=500;
				else if (board_state[p.second][p.first] == (1))
					{
						sattack+=20;
						if(p.second +1 <n && (board_state[p.second+1][p.first])==1 )
							sattack -= 15;
						if(p.second +1 <n && (p.first-1 >= 0) && (board_state[p.second+1][p.first-1])==1 )
							sattack -= 15;
						if(p.first-1 >=0 && (board_state[p.second][p.first-1]==1))
							sattack-= 15;
						if(p.first+1 <m && (board_state[p.second][p.first+1]==1))
							sattack-= 15;						
						if(p.second -1 >= 0 && (p.first+1 < m) && (board_state[p.second-1][p.first+1])==1 )
							sattack -= 15;
					}
				else
					snot_attack++;
			}

		}
	}
}

vector<pair<short, short>> state::get_soldier_moves(pair<short,short> pos)
{
	vector<pair<short, short>> moves;
	if (color == 0)
	{
		if (pos.second >= 0)
		{

			if (pos.first > 0)
			{
				if (board_state[pos.second][pos.first - 1] < 0)
					moves.push_back(make_pair(pos.first - 1, pos.second));
				if((pos.second-1>=0) && board_state[pos.second - 1 ][pos.first -1] <= 0)
				 moves.push_back(make_pair(pos.first - 1, pos.second - 1));


			}
			if (pos.first < (m-1)) 
			{
				if((pos.second-1 >= 0) && board_state[pos.second - 1 ][pos.first + 1] <= 0)
					moves.push_back(make_pair(pos.first + 1, pos.second - 1));
				if (board_state[pos.second][pos.first + 1] < 0)
					moves.push_back(make_pair(pos.first + 1, pos.second));
			}
			if((pos.second-1) >= 0 && board_state[pos.second - 1 ][pos.first] <= 0)
			 moves.push_back(make_pair(pos.first, pos.second - 1));
		}

		if (adjacent(pos))
		{
			if (pos.second + 2 < n)
			{
				if(board_state[pos.second + 2 ][pos.first] <= 0)
				   moves.push_back(make_pair(pos.first, pos.second + 2));
				if ((pos.first - 2) >= 0 && board_state[pos.second + 2 ][pos.first - 2 ] <= 0)
					moves.push_back(make_pair(pos.first - 2, pos.second + 2));
				if ((pos.first + 2) < m && board_state[pos.second + 2 ][pos.first + 2] <= 0)
					moves.push_back(make_pair(pos.first + 2, pos.second + 2));
			}
		}



	}
	else
	{
		if (pos.second <= (n-1))
		{
			if((pos.second+1 < n) && board_state[pos.second + 1][pos.first] >= 0)
			 moves.push_back(make_pair(pos.first, pos.second + 1));
			if (pos.first > 0)
			{
				if(((pos.second + 1) < n) && board_state[pos.second + 1][pos.first -1] >= 0)
				 moves.push_back(make_pair(pos.first - 1, pos.second + 1));
				if (board_state[pos.second][pos.first - 1] > 0)
					moves.push_back(make_pair(pos.first - 1, pos.second));

			}
			if (pos.first < (m-1))
			{
				if( ((pos.second + 1) < n) && board_state[pos.second + 1][pos.first + 1] >= 0)
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
				if ((pos.first + 2) < m && board_state[pos.second - 2][pos.first + 2] >= 0)
					moves.push_back(make_pair(pos.first + 2, pos.second - 2));
			}
		}
	}

	return moves;
}

bool state::adjacent(pair<short, short> pos)
{

	if(color == 0)
	{
		if(pos.second >0)
		{
			if(board_state[pos.second -1][pos.first] == -1)
			 return true;
			if(pos.first > 0 && ((board_state[pos.second -1][pos.first -1] == -1)||(board_state[pos.second][pos.first-1] == -1)) )
			 return true;
			if(pos.first < (m-1) && ((board_state[pos.second -1][pos.first +1] == -1)||(board_state[pos.second][pos.first+1]== -1)) )
			 return true;
		}
		else if(pos.first > 0 && (board_state[pos.second][pos.first -1] == -1))
		 return true;
		else if(pos.first < (m-1) && (board_state[pos.second][pos.first +1] == -1))
		 return true;

	}
	else
	{
		if(pos.second < (n-1))
		{
			if(board_state[pos.second +1][pos.first] == 1)
			 return true;
			if(pos.first > 0 && ((board_state[pos.second +1][pos.first -1] == 1)||(board_state[pos.second][pos.first-1]==1)) )
			 return true;
			if(pos.first < (m-1) && ((board_state[pos.second +1][pos.first +1] == 1)||(board_state[pos.second][pos.first+1] ==1)) )
			 return true;
		}
		else if(pos.first > 0 && (board_state[pos.second][pos.first -1] == 1))
		 return true;
		else if(pos.first < (m-1) && (board_state[pos.second][pos.first +1] == 1))
		 return true;
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
			if (c.second > 0 && c.second < (n-1) && (board_state[c.second + 1][c.first] == 1) && (board_state[c.second -1][c.first] == 1))
				cannons.push_back(make_pair(0, c));
			if (c.first > 0 && c.first < (m-1) && (board_state[c.second][c.first+1] == 1) && (board_state[c.second][c.first-1] == 1))
				cannons.push_back(make_pair(1, c));
			if ((c.first > 0) && (c.second > 0) && (c.first < (m-1)) && (c.second < (n-1)) && (board_state[c.second - 1][c.first + 1] ==1) && (board_state[c.second + 1][c.first- 1] == 1))
				cannons.push_back(make_pair(2, c));
			if ((c.first > 0) && (c.second > 0) && (c.first < (m-1)) && (c.second < (n-1)) && (board_state[c.second + 1][c.first + 1] == 1) && (board_state[c.second - 1][c.first - 1] == 1))
				cannons.push_back(make_pair(3, c));

		}
	}
	else
	{
		for (auto c : soldiers)
		{
			if (c.second > 0 && c.second < (n-1) && (board_state[c.second + 1][c.first] == -1) && (board_state[c.second -1][c.first] == -1))
				cannons.push_back(make_pair(0, c));
			if (c.first > 0 && c.first < (m-1) && (board_state[c.second][c.first+1] == -1) && (board_state[c.second][c.first-1] == -1))
				cannons.push_back(make_pair(1, c));
			if ((c.first > 0) && (c.second > 0) && (c.first < (m-1)) && (c.second < (n-1)) && (board_state[c.second - 1][c.first + 1] == -1) && (board_state[c.second + 1][c.first- 1] == -1))
				cannons.push_back(make_pair(2, c));
			if ((c.first > 0) && (c.second > 0) && (c.first < (m-1)) && (c.second < (n-1)) && (board_state[c.second + 1][c.first + 1] == -1) && (board_state[c.second - 1][c.first - 1] == -1))
				cannons.push_back(make_pair(3, c));

		}

	}
	return cannons;
}

float state::miniMaxPrune(float alpha, float beta)
{

	cannons = get_cannon_pos();
	float val;
	if (color == ourPlayer)
		val = -1000000000;
	else
		val = 1000000000;
	
	for (auto c : cannons)
	{
		//cout<<"HERE CANNON ";
		auto moves = get_cannon_moves(c);
		//cout<<"HERE CANNON MOVES ";
		for (auto p : moves)
		{
			//cout<<"HERE CANNON ";
			//print_board();
			//print_this_move(p);
			child_states.push_back(get_next_state(p,alpha,beta));
			if (color == ourPlayer)
			{
				if (val < child_states.back()->eval_value)
				{
					fro_move = p;
				}
				val = max(val, child_states.back()->eval_value);
				if (val > beta || (alpha>=beta))
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
				if (val < alpha || (alpha>=beta))
					return val;
				beta = min(beta, val);
			}

		}
	}

	for (auto c : soldiers)
	{
		auto moves = get_soldier_moves(c);
		for (auto p : moves)
		{
			//print_board();
			//print_this_move(make_tuple(0,c,p));
			child_states.push_back(get_next_state(make_tuple(0,c,p), alpha, beta));
			if (color == ourPlayer)
			{
				if (val < child_states.back()->eval_value)
				{
					fro_move = make_tuple(0,c,p);
				}

				val = max(val, child_states.back()->eval_value);
				if (val > beta || (alpha>=beta))
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
				if (val < alpha || (alpha>=beta))
					return val;
				beta = min(beta, val);
			}

		}
	}

	eval_value = val;
	//print_board();
	//outfile<<val<<endl;
	//print_this_move(fro_move);
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
			if (pos.second > 1 && (pos.second + 1 < n) && board_state[pos.second - 2][pos.first] == 0)
				moves.push_back(make_tuple(0, make_pair(pos.first, pos.second + 1), make_pair(pos.first, pos.second - 2)));
			if (pos.second < (n-2) && (pos.second - 1 >= 0) && board_state[pos.second+2][pos.first] == 0)
				moves.push_back(make_tuple(0, make_pair(pos.first, pos.second - 1), make_pair(pos.first, pos.second + 2)));

			if (pos.second > 2 && board_state[pos.second - 3][pos.first] < 0 && board_state[pos.second - 2][pos.first] == 0)
			{
				moves.push_back(make_tuple(1,pos,make_pair(pos.first,pos.second - 3)));
			}
			if (pos.second < (n-3) && board_state[pos.second + 3][pos.first] < 0 && board_state[pos.second + 2][pos.first] == 0)
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first, pos.second + 3)));
			}
			if (pos.second > 3 && board_state[pos.second - 4][pos.first] < 0 && board_state[pos.second - 2][pos.first] == 0 )
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first, pos.second - 4)));
			}
			if (pos.second < (n-4) && board_state[pos.second + 4][pos.first] < 0 && board_state[pos.second + 2][pos.first] == 0 )
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first, pos.second + 4)));
			}


		}
		else if (cannon.first == 1)
		{
			if (pos.first > 2 && board_state[pos.second][pos.first -3] < 0 && board_state[pos.second][pos.first - 2] == 0)
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first -3, pos.second)));
			}
			if (pos.first < (m-3) && board_state[pos.second][pos.first+3] < 0 && board_state[pos.second][pos.first + 2] == 0)
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first + 3, pos.second)));
			}
			if (pos.first > 3 && board_state[pos.second][pos.first-4] < 0 && board_state[pos.second][pos.first - 2] == 0 )
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first - 4, pos.second)));
			}
			if (pos.first < (m-4) && board_state[pos.second][pos.first+4] < 0 &&  board_state[pos.second][pos.first + 2] == 0)
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first + 4, pos.second)));
			}

			if (pos.first > 1 && (pos.first + 1 < m) && board_state[pos.second][pos.first-2] == 0)
				moves.push_back(make_tuple(0, make_pair(pos.first + 1, pos.second), make_pair(pos.first - 2 , pos.second)));
			if (pos.first < (m-2) && (pos.first - 1 >= 0) &&  board_state[pos.second][pos.first+2] == 0)
				moves.push_back(make_tuple(0, make_pair(pos.first - 1, pos.second), make_pair(pos.first + 2, pos.second)));
		}
		else if (cannon.first == 2)
		{
			if ((pos.second - 3 ) >= 0 && ((pos.first + 3) < m) && (board_state[pos.second -3][pos.first + 3] < 0) && (board_state[pos.second -2][pos.first + 2] == 0))
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first + 3, pos.second - 3)));
			}
			if ((pos.second + 3) <n && ((pos.first - 3) >=0) && (board_state[pos.second + 3][pos.first - 3] < 0) && (board_state[pos.second +2][pos.first - 2] == 0))
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first -3, pos.second + 3)));
			}
			if ((pos.second - 4) >= 0 && ((pos.first + 4) < m) && (board_state[pos.second - 4][pos.first + 4] < 0) && (board_state[pos.second -2][pos.first + 2] == 0))
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first + 4, pos.second - 4)));
			}
			if ((pos.second + 4) < n && ((pos.first - 4) >= 0) && (board_state[pos.second + 4][pos.first - 4] < 0)&& (board_state[pos.second +2][pos.first - 2] == 0))
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first - 4, pos.second + 4)));
			}

			if (((pos.second - 2) >= 0) && ((pos.first +2) <m ) && board_state[pos.second - 2][pos.first + 2] == 0)
				moves.push_back(make_tuple(0, make_pair(pos.first - 1, pos.second + 1), make_pair(pos.first + 2, pos.second - 2)));
			if (((pos.second + 2) < n) && ((pos.first - 2) >= 2) && board_state[pos.second +2][pos.first -2] == 0)
				moves.push_back(make_tuple(0, make_pair(pos.first + 1, pos.second - 1), make_pair(pos.first - 2, pos.second + 2)));
		}
		else if (cannon.first == 3)
		{
			if ((pos.second - 3) >= 0 && ((pos.first - 3) >= 0) && (board_state[pos.second- 3][pos.first - 3] < 0) && (board_state[pos.second -2][pos.first - 2] == 0))
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first - 3, pos.second - 3)));
			}
			if ((pos.second + 3) < n && ((pos.first + 3) < m) && (board_state[pos.second + 3][pos.first + 3] < 0)&& (board_state[pos.second +2][pos.first + 2] == 0))
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first + 3, pos.second + 3)));
			}
			if ((pos.second - 4) >= 0 && ((pos.first - 4) >= 0) && (board_state[pos.second - 4][pos.first - 4] < 0)&& (board_state[pos.second -2][pos.first - 2] == 0))
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first - 4, pos.second - 4)));
			}
			if ((pos.second + 4) < n && ((pos.first + 4) < m) && (board_state[pos.second + 4][pos.first + 4] < 0) && (board_state[pos.second +2][pos.first + 2] == 0))
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first + 4, pos.second + 4)));
			}

			if (((pos.second - 2) >= 0) && ((pos.first - 2) >= 0) && board_state[pos.second - 2][pos.first - 2] == 0)
				moves.push_back(make_tuple(0, make_pair(pos.first + 1, pos.second + 1), make_pair(pos.first - 2, pos.second - 2)));
			if (((pos.second + 2) < n) && ((pos.first + 2) < m) && board_state[pos.second + 2][pos.first + 2] == 0)
				moves.push_back(make_tuple(0, make_pair(pos.first - 1, pos.second - 1), make_pair(pos.first + 2, pos.second + 2)));
		}
	}
	else
	{
		if (cannon.first == 0)
		{
			

			if (pos.second > 2 && board_state[pos.second - 3][pos.first] > 0 && board_state[pos.second - 2][pos.first] == 0)
			{
				moves.push_back(make_tuple(1,pos,make_pair(pos.first,pos.second - 3)));
			}
			if (pos.second < (n-3) && board_state[pos.second + 3][pos.first] > 0 && board_state[pos.second + 2][pos.first] == 0)
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first, pos.second + 3)));
			}
			if (pos.second > 3 && board_state[pos.second - 4][pos.first] > 0 && board_state[pos.second - 2][pos.first] == 0)
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first, pos.second - 4)) );
			}
			if (pos.second < (n-4) && board_state[pos.second + 4][pos.first] > 0 && board_state[pos.second + 2][pos.first] == 0)
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first, pos.second + 4)) );
			}
			if (pos.second > 1 && (pos.second + 1 < n) &&  board_state[pos.second - 2][pos.first] == 0)
				moves.push_back(make_tuple(0, make_pair(pos.first, pos.second + 1), make_pair(pos.first, pos.second - 2)));
			if (pos.second < (n-2) && (pos.second - 1 >= 0) && board_state[pos.second+2][pos.first] == 0)
				moves.push_back(make_tuple(0, make_pair(pos.first, pos.second - 1), make_pair(pos.first, pos.second + 2)));


		}
		else if (cannon.first == 1)
		{
			if (pos.first > 2 && board_state[pos.second][pos.first -3] > 0 && board_state[pos.second][pos.first-2] == 0)
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first -3, pos.second)));
			}
			if (pos.first < (m-3) && board_state[pos.second][pos.first+3] > 0 && board_state[pos.second][pos.first+2] == 0)
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first + 3, pos.second)));
			}
			if (pos.first > 3 && board_state[pos.second][pos.first-4] > 0 && board_state[pos.second][pos.first-2] == 0)
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first - 4, pos.second)));
			}
			if (pos.first < (m-4) && board_state[pos.second][pos.first+4] > 0 && board_state[pos.second][pos.first+2] == 0)
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first + 4, pos.second)));
			}

			if (pos.first > 1 && (pos.first + 1 < m) && board_state[pos.second][pos.first-2] == 0)
				moves.push_back(make_tuple(0, make_pair(pos.first + 1, pos.second), make_pair(pos.first - 2 , pos.second)));
			if (pos.first < (m-2) && (pos.second - 1 >= 0) &&  board_state[pos.second][pos.first+2] == 0)
				moves.push_back(make_tuple(0, make_pair(pos.first - 1, pos.second), make_pair(pos.first + 2, pos.second)));
		}
		else if (cannon.first == 2)
		{
			if ((pos.second - 3 ) >= 0 && ((pos.first + 3) < m) && (board_state[pos.second -3][pos.first + 3] > 0)  && board_state[pos.second - 2][pos.first + 2] == 0)
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first + 3, pos.second - 3)));
			}
			if ((pos.second + 3) <n && ((pos.first - 3) >=0) && (board_state[pos.second + 3][pos.first - 3] > 0)  && board_state[pos.second + 2][pos.first - 2] == 0)
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first -3, pos.second + 3)));
			}
			if ((pos.second - 4) >= 0 && ((pos.first + 4) < m) && (board_state[pos.second - 4][pos.first + 4] > 0)  && board_state[pos.second - 2][pos.first + 2] == 0)
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first + 4, pos.second - 4)));
			}
			if ((pos.second + 4) < n && ((pos.first - 4) >= 0) && (board_state[pos.second + 4][pos.first - 4] > 0)  && board_state[pos.second + 2][pos.first - 2] == 0)
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first - 4, pos.second + 4)));
			}

			if (((pos.second - 2) >= 0) && ((pos.first +2) <m ) && board_state[pos.second - 2][pos.first + 2] == 0)
				moves.push_back(make_tuple(0, make_pair(pos.first - 1, pos.second + 1), make_pair(pos.first + 2, pos.second - 2)));
			if (((pos.second + 2) < n) && ((pos.first - 2) >= 2) && board_state[pos.second +2][pos.first -2] == 0)
				moves.push_back(make_tuple(0, make_pair(pos.first + 1, pos.second - 1), make_pair(pos.first - 2, pos.second + 2)));
		}
		else if (cannon.first == 3)
		{
			if ((pos.second - 3) >= 0 && ((pos.first - 3) >= 0) && (board_state[pos.second- 3][pos.first - 3] > 0) && board_state[pos.second - 2][pos.first - 2] == 0)
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first - 3, pos.second - 3)));
			}
			if ((pos.second + 3) < n && ((pos.first + 3) < m) && (board_state[pos.second + 3][pos.first + 3] > 0) && board_state[pos.second + 2][pos.first + 2] == 0)
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first + 3, pos.second + 3)));
			}
			if ((pos.second - 4) >= 0 && ((pos.first - 4) >= 0) && (board_state[pos.second - 4][pos.first - 4] > 0) && board_state[pos.second - 2][pos.first - 2] == 0)
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first - 4, pos.second - 4)));
			}
			if ((pos.second + 4) < n && ((pos.first + 4) < m) && (board_state[pos.second + 4][pos.first + 4] > 0) && board_state[pos.second + 2][pos.first + 2] == 0)
			{
				moves.push_back(make_tuple(1, pos, make_pair(pos.first + 4, pos.second + 4)));
			}

			if (((pos.second - 2) >= 0) && ((pos.first - 2) >= 0) && board_state[pos.second - 2][pos.first - 2] == 0)
				moves.push_back(make_tuple(0, make_pair(pos.first + 1, pos.second + 1), make_pair(pos.first - 2, pos.second - 2)));
			if (((pos.second + 2) < n) && ((pos.first + 2) < m) && board_state[pos.second + 2][pos.first + 2] == 0)
				moves.push_back(make_tuple(0, make_pair(pos.first - 1, pos.second - 1), make_pair(pos.first + 2, pos.second + 2)));
		}
	}

	return moves;
}

void state::print_this_move(tuple<short, pair<short, short>, pair<short, short>> move_s)
{
	if (get<0>(move_s) == 0)
	{
		outfile << "S " << get<1>(move_s).first << " " << get<1>(move_s).second << " M " << get<2>(move_s).first << " " << get<2>(move_s).second << endl;
	}
	else
	{
		outfile << "S " << get<1>(move_s).first << " " << get<1>(move_s).second << " B " << get<2>(move_s).first << " " << get<2>(move_s).second << endl;
	}
}

float state::evaluate()
{
	float res=0;
	if(color==ourPlayer)
	{
		for(int i=0;i<our_weights.size()-2;i++)
			res+=(our_weights[i]*our_vals[i]);

	}
	else
	{
		for(int i=0;i<other_weights.size()-2;i++)
			res+=(other_weights[i]*other_vals[i]);
	}
	
	return (10*cattack+cnot_attack+sattack+snot_attack+5*townhall_attack+ 15000*townhalls + 100*soldiers.size() + 20*cannons.size());
}

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
	clear_childs();
    this->color = ncol;
	depth = 0;
	this->board_state = get_next_board(fro_move);
	child_states.clear();
	this-> moves = this->moves + 1;
	return this;
}


state* state::get_next_state(tuple<short, pair<short, short>, pair<short, short>> move,float alpha,float beta)
{
	short ncol;

	if (color == 1)
		ncol = 0;
	else
		ncol = 1;

	state * new_state = new state(board_state, move, depth + 1, ncol,ourPlayer,alpha,beta,moves + 1,n,m);

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
	outfile<<"Borad"<<endl;
	for (short i = 0; i < n; i++)
	{
		for (short j = 0; j < m; j++)
			outfile << board_state[i][j] << " ";
		outfile << endl;
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
	if(n==10 && m==8)
	{
		board_state = { {-2,-1,-2,-1,-2,-1,-2,-1},
		{0,-1,0,-1,0,-1,0,-1},
		{0,-1,0,-1,0,-1,0,-1},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{1,0,1,0,1,0,1,0},
		{1,0,1,0,1,0,1,0},
		{1,2,1,2,1,2,1,2},
		};
	}
	if(n==10 && m==10)
	{
		board_state = {{-2,-1,-2,-1,-2,-1,-2,-1,-2,-1},
		{0,-1,0,-1,0,-1,0,-1,0,-1},
		{0,-1,0,-1,0,-1,0,-1,0,-1},
		{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},
		{1,0,1,0,1,0,1,0,1,0},
		{1,0,1,0,1,0,1,0,1,0},
		{1,2,1,2,1,2,1,2,1,2},
		};
	}
	//ofstream outfi("l.txt");
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
		game = new state(board_state, nex_move, 0, 1, 1, -10000, 10000, 1,m,n);
		game->print_move();
		game = game->get_next_state();
	}
	else
	{
		game = new state(m,n);
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
		board_state = game->board_state;
		delete(game);
		game = new state(board_state, nex_move, 0, ourName, ourName, -10000000, 10000000, 1,n,m);
		game->print_move();
		game = game->get_next_state();
		if(game->check_State())
			return 0;
	}
	return 0;

}
