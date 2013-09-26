#include "config.h"

using namespace std;

fstream config::input;
fstream config::jobs;
fstream config::users;
fstream config::deadlines;
fstream config::consumption;

long long config::cpu_capacity;
long long config::ram_capacity;


vector<string> config::ignored_queues;
vector<string> config::ignored_users;

int config::minimumJobs;
int config::minimumDeadlines;
int config::minimumDeadlineTime;

bool config::generateHeatmap;

string config::input_filename;

string config::single_queue_mode;

string config::heatmap_filename;

