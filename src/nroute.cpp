#include "headers/nroute.hpp"

using namespace std;

nroute::nroute() : rtime(time(nullptr)), status(discover_status::empty) {
    // simply "null oath"
    std::time(&rtime);
    this->responses = 0;
    this->ignore_id = 0;
}

void nroute::update(NodeMessage &msg) {
    if (msg.function_id == net_fid::discover) {
        remove_ignore_id();
    }

    if (msg.function_id == net_fid::nack or msg.function_id == net_fid::discover) {
        if (this->responded[msg.source_id] == false) {
            this->responded[msg.source_id] = true;
            time(&this->rtime);
            responses++;

            if (msg.function_id == net_fid::discover) {
                ignore_id = msg.source_id;
            }
        }
    } else if (msg.function_id == net_fid::route) {
        if (this->responded[msg.source_id] == false) {
            this->responded[msg.source_id] = true;
            time(&this->rtime);
            responses++;
        }
        int *arr = (int *)msg.payload;
        deque<int> npath;
        uint size = arr[1];
        for (uint i = 0; i < size; i++) {
            npath.push_back(arr[i + 2]);
        }
        if (this->status == discover_status::empty or this->path.size() > npath.size()) {
            this->path = npath;
        } else {
            bool swap = false;
            for (uint i = 0; i < size; i++) {
                if (npath[i] < this->path[i]) {
                    swap = true;
                    break;
                } else if (npath[i] > this->path[i]) {
                    break;
                }
            }
            if (swap) {
                this->path.swap(npath);
                //this->path = npath;
            }
        }
        this->status = discover_status::found;
    }
}

bool nroute::check_valid(int timeout) {
    time_t current;
    time(&current);
    double dif = difftime(this->rtime, current);
    if (dif > timeout) {
        // if is expired we simply "reset" the records.
        this->dump();
        return false;
    }
    return true;
}

void nroute::dump() {
    this->status = discover_status::empty;
    this->responded.clear();
    this->path.clear();
    this->ignore_id = 0;
    this->responses = 0;
}

std::string nroute::to_string() {
    std::string p;
    for (uint i = 0; i < this->path.size(); i++) {
        if (i > 0) {
            p += "->";
        }
        p += std::to_string(this->path[i]);
    }
    return p;
}

void nroute::pack_msg(NodeMessage &msg) {
    int *arr = (int *)msg.payload;
    arr[1] = this->path.size();
    for (uint i = 0; i < this->path.size(); i++) {
        arr[i + 2] = this->path[i];
    }
}

void nroute::remove_ignore_id() {
    if (this->ignore_id != 0) {
        if (this->responded[this->ignore_id] == true) {
            this->responded[this->ignore_id] = false;
            responses--;
        }
    }
}
