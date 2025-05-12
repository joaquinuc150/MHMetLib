#pragma once

#include <algorithm>

static std::string strstrip(std::string s)
{
  if (s.empty()) {
      return s;
  }
  s.erase(0,s.find_first_not_of(' '));
  /// For string line end with '\r' in windows systems.
  s.erase(s.find_last_not_of('\r') + 1);
  s.erase(s.find_last_not_of(' ') + 1);
  return s;
}


static std::vector<int> get_int_vector_parse_string(std::string input, const int _min, const int _max) {
  std::vector<std::string> spiltstring;
  std::string tmp;
  int tmpvalue,tmpvalue1;
  std::vector<int> result;

  size_t n = input.size();
  input = strstrip(input);
  for (size_t i = 0; i < n; ++i) {
    if (input[i] != ',' && input[i] != '-' && !isdigit(input[i])) {
      cerr << "The configuration consists invalid characters.";
    }
  }
  
  std::stringstream raw(input);
  while(getline(raw, tmp, ',')) spiltstring.push_back(tmp);

  n = spiltstring.size();
  for (size_t i = 0; i < n; ++i) {
    //size_t l = spiltstring[i].size();

    if (spiltstring[i][0] == '-') {
      /// The condition beginning with "-m"
      if(i != 0) {
        cerr << "Format error in configuration.";
      } else {
        tmp = spiltstring[i].substr(1);
        if (tmp.find('-') != std::string::npos) {
          cerr << "Format error in configuration.";
        }
        
        tmpvalue = std::stoi(tmp);
        
        if (tmpvalue < _min) {
          cerr << "Input value exceeds lowerbound.";
        }

        for (int value = _min; value <= tmpvalue; ++value) {
          result.push_back(value);
        }
      }
    } else if(spiltstring[i][spiltstring[i].length()-1] == '-') {
      /// The condition endding with "n-"
      if (i != spiltstring.size() - 1) {
        cerr << "Format error in configuration.";
      } else {
        tmp = spiltstring[i].substr(0,spiltstring[i].length() - 1);
        if (tmp.find('-') != std::string::npos) {
          cerr << "Format error in configuration.";
        }
        tmpvalue = std::stoi(tmp);
        if (tmpvalue > _max) {
          cerr << "Input value exceeds upperbound.";
        }
        for (int value = _max; value <= tmpvalue; --value) {
          result.push_back(value);
        }
      }
    } else {
      /// The condition with "n-m,n-x-m"
      std::stringstream tempraw(spiltstring[i]);
      std::vector<std::string> tmpvaluevector;
      while (getline(tempraw, tmp, '-')) {
        tmpvaluevector.push_back(tmp);
      }
      tmpvalue = std::stoi(tmpvaluevector[0]);
      tmpvalue1 = std::stoi(tmpvaluevector[tmpvaluevector.size()-1]);
      if (tmpvalue > tmpvalue1) {
        cerr << "Format error in configuration.";
      }
      if (tmpvalue < _min)  {
        cerr << "Input value exceeds lowerbound.";
      }
      if (tmpvalue1 > _max) {
        cerr << "Input value exceeds upperbound.";
      }
      for(int value = tmpvalue; value <= tmpvalue1; ++value) result.push_back(value);
    } 
  }
  return result;
}

std::vector<double> sum_vector(std::vector<double> a, std::vector<double> b) {
    std::transform(a.begin(), a.end(), b.begin(), a.begin(), std::plus<int>());
    return a;
}

std::vector<double> move_domain(std::vector<double> f) {
    double min = *std::min_element(f.begin(), f.end());
    if (min < 0) {
        std::transform(f.begin(), f.end(), f.begin(), [min](double &c) { return c + abs(min); });
    }
    return f;
}

