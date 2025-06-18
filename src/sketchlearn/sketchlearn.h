#ifndef __SKETCHLEARN_H__
#define __SKETCHLEARN_H__
#include <linux/types.h>
#include <cstdint>
#include <map>
#include <string>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <set>
#include <algorithm>
#include "twotuple.h"
#include "../../include/xxhash.h"

using namespace std;

template<int l, int r, int c> // l levels, r rows, c columns

class SketchLearn
{
private:
    uint32_t counters[l+1][r][c];
    // per-bit distribution
    double theta = 0.5;
    double p[l+1]; // level k: p[k], sigma2[k]
    double sigma2[l+1];
    // large flow list
    map<struct twoTuple_t, uint32_t> flow_list_twotpl;
    map<string, uint32_t> flow_list_str;
public:
    SketchLearn() {
        memset(&this->counters, 0, sizeof(this->counters));
        memset(&this->p, 0.0, sizeof(this->p));
        memset(&this->sigma2, 0.0, sizeof(this->sigma2));
        srand(time(0));
    }

    // update multi-level sketch
    void insert(uint32_t srcIP, uint32_t dstIP, int inc = 1) {
        u_int64_t key = combine_IPs(srcIP, dstIP);
        string key_str = get_bits_from_key(key);

        // update level 0
        for (int i = 0; i < r; i++) {
            int index = two_tuple_sketch_hash(srcIP, dstIP, i, c);
            counters[0][i][index] += inc; 
        }

        // update level [1,l]
        for (int k = 1; k < l+1; k++) { // for k-th bit of the key
            if (key_str[k-1] == '1') {
                for (int i = 0; i < r; i++) {
                    int index = two_tuple_sketch_hash(srcIP, dstIP, i, c);
                    counters[k][i][index] += inc;
                }
            }
        }
    }

    void compute_dist() {
        // cleanup arrays
        memset(&this->p, 0.0, sizeof(this->p));
        memset(&this->sigma2, 0.0, sizeof(this->sigma2));

        // compute distribution for k-th level
        for (int k = 0; k < l+1; k++) {
            // calculate mean
            int sum = 0;
            for (int i = 0; i < r; i++) {
                for (int j = 0; j < c; j++) {
                    sum += this->counters[k][i][j];
                }
            }
            this->p[k] = sum*1.0 / r*c*1.0;
            // calculate sigma2
            double var = 0.0;
            for (int i = 0; i < r; i++) {
                for (int j = 0; j < c; j++) {
                    var += (this->counters[k][i][j]-this->p[k])*(this->counters[k][i][j]-this->p[k])*1.0;
                }
            }
            this->sigma2[k] = var*1.0 / r*c*1.0;
        }
    }

    void get_strings_from_template(vector<string>& arr, string T) {
        queue<string> unfinished_keys;
        unfinished_keys.push(T);
        while (!unfinished_keys.empty()) {
            string unfinished_key = unfinished_keys.front();
            unfinished_keys.pop();
            bool not_found = true;
            for (int i = 0; i < unfinished_key.length(); i++) {
                if (unfinished_key[i] == '*') {
                    string unfinished_key1 = unfinished_key;
                    string unfinished_key2 = unfinished_key;
                    unfinished_key1[i] = '1';
                    unfinished_key2[i] = '0';
                    unfinished_keys.push(unfinished_key1);
                    unfinished_keys.push(unfinished_key2);
                    not_found = false;
                    break;
                }
            }
            if (not_found) arr.push_back(unfinished_key);
        }
        set<string> s;
        unsigned size = arr.size();
        for ( unsigned i = 0; i < size; ++i ) s.insert( arr[i] );
        arr.assign(s.begin(), s.end());
    }

    vector<twoTuple_t> extract_large_flows(int i, int j) {
        // Step#1: Estimating bit-level probabilities (p_)
        double p_[l+1];
        double Rij[l+1];
        memset(&p_, 0, sizeof(p_));
        memset(&Rij, 0, sizeof(Rij));
        for (int k = 1; k < l+1; k++) {
            Rij[k] = this->counters[k][i][j]*1.0 / this->counters[0][i][j];
            if (Rij[k] < this->theta) p_[k] = 0;
            else if ((1-Rij[k]) < this->theta) p_[k] = 1;
            else { // Bayes’ Theorem
                // TODO: acquire residual Rij[k] as assuming p_[k] = 1
                p_[k] = 1-Rij[k];
            }
        } 

        // Step#2: Finding candidate flowkeys
        vector<twoTuple_t> candidate;
        vector<string> candidate_str;
        string T = "";
        for (int k = 1; k < l+1; k++) {
            if (p_[k] > 0.99) {
                T.append("1");
            } else if (1-p_[k] > 0.99) {
                T.append("0");
            } else {
                T.append("*");
            }
        }

        vector<string> keys; 
        get_strings_from_template(keys, T); // get all strings from template T

        for (int m = 0; m < keys.size(); m++) {
            string key_str = keys[m];
            // convert bits to uint32
            uint32_t srcIP=0, dstIP=0;
            get_key_from_bits(key_str, &srcIP, &dstIP);
            // if (hi(f) == j) // if the hash of flowkey equals to j
            int j_ = two_tuple_sketch_hash(srcIP, dstIP, i, c);
            if (j_ == j) {
                twoTuple_t* key_twotpl_p = (twoTuple_t*)malloc(sizeof(twoTuple_t));
                candidate.push_back(*(key_twotpl_p));
                candidate_str.push_back(key_str);
            }
        }

        // Step#3: Estimating frequencies
        double e[l+1];
        double sum = 0;
        memset(&e, 0.0, sizeof(e));
        for (int m = 0; m < candidate.size(); m++) {
            twoTuple_t f = candidate[m];
            string f_str = candidate_str[m];
            for (int k = 1; k < l+1; k++) {
                if (f_str[k-1] == '1') {
                    e[k] = ((Rij[k]-p[k])*1.0/(1-p[k]) * this->counters[0][i][j]);
                } else if (f_str[k-1] == '0') {
                    e[k] = ((1-(Rij[k]*1.0/p[k]*1.0)) * this->counters[0][i][j]);
                }
                sum += e[k];
            }
        }
        double s_f = sum*1.0/l;
        cout << s_f << endl;

        // TODO: Step#4: Associating flowkeys with bit-level probabilities

        // Step#5: Verifying candidate flows
        for (int m = 0; m < candidate.size(); m++) {
            twoTuple_t f = candidate[m];
            string f_str = candidate_str[m];
            for (int i_ = 0; i_ < r; i_++) {
                if (i_ == i) continue;
                int j_ = two_tuple_sketch_hash(f.srcIP, f.dstIP, i_, c);
                for (int k = 1; k < l+1; k++) {
                    if (f_str[k-1] == '0' && (this->counters[0][i_][j_] - this->counters[k][i_][j_]) < s_f) {
                        s_f = this->counters[0][i_][j_] - this->counters[k-1][i_][j_];
                    } else if (f_str[k-1] == '1' && this->counters[k][i_][j_] < s_f) {
                        s_f = this->counters[k][i_][j_];
                    }
                } 
            }
            if (s_f < this->theta*this->counters[0][i][j]) {
                candidate.erase(std::remove(candidate.begin(), candidate.end(), f), candidate.end());
                candidate_str.erase(std::remove(candidate_str.begin(), candidate_str.end(), f_str), candidate_str.end());
            }
        }

        // join flow to flow_list
        for (int m = 0; m < candidate.size(); m++) {
            twoTuple_t f = candidate[m];
            string f_str = candidate_str[m];
            this->flow_list_twotpl.insert({f, s_f});
            this->flow_list_str.insert({f_str, s_f});
            cout << f.srcIP << " " << f.dstIP << " " << s_f << endl;
        }

        return candidate;
    }

    void model_inference(int inc = 1) {
        // Bit-level counter distributions
        compute_dist();
        // while (true) {
            // Set of extracted large flows
            vector<twoTuple_t> candidate;
            // for all stack (i, j), 1 ≤ i ≤ r , 1 ≤ j ≤ c do
            for (int i = 0; i < r; i++) {
                for (int j = 0; j < c; j++) {
                    vector<twoTuple_t> subcandidate = extract_large_flows(i, j);
                    candidate.insert(candidate.end(), subcandidate.begin(), subcandidate.end());
                }
            }
            // RemoveFlows(S, F')
            for (int i = 0; i < candidate.size(); i++) {
                twoTuple_t f = candidate[i];
                u_int64_t f_uint64 = combine_IPs(f.srcIP, f.dstIP);
                string f_str = get_bits_from_key(f_uint64);

                // update level 0
                for (int j = 0; j < r; j++) {
                    int index = two_tuple_sketch_hash(f.srcIP, f.dstIP, j, c);
                    counters[0][j][index] -= inc; 
                }

                // update level [1,l]
                for (int k = 1; k < l+1; k++) { // for k-th bit of the key
                    if (f_str[k-1] == 1) {
                        for (int j = 0; j < r; j++) {
                            int index = two_tuple_sketch_hash(f.srcIP, f.dstIP, j, c);
                            counters[k][j][index] -= inc;
                        }
                    }
                }
            }
            compute_dist();

        // }
    }

    int query(uint32_t srcIP, uint32_t dstIP) {
        
    }

};

#endif