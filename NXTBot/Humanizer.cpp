#include "pch.h"
#include "GameClasses.h"
#include "Humanizer.h"


humanizer::humanizer(const char* dataFile)
{
	std::random_device rd; mt = std::mt19937(rd());

	if (!read_biometric_file(dataFile))
	{
		printf("[-] Mouse data file does not exist\n");
		return;
	}

	if (!populate_delay_points() || !populate_segments() || !populate_segments_probability())
	{
		printf("[-] Failed to init vector datas\n");
		return;
	}

	if (this->segments.size() == 0)
	{
		printf("[-] Segments is empty.\n");
		return;
	}

	delete[] mouse_data; mouse_data = 0;
	printf("[+] Successfully initialized humanizer with (%d) segments.\n", this->segments.size());
	return;
}

humanizer::~humanizer()
{

}

// segment

segment::segment(int lower_bound, int upper_bound)
{
	this->lower_bound = lower_bound;
	this->upper_bound = upper_bound;

	this->mean = 0.0f;
	this->standard_deviation = 0.0f;

	std::random_device rd;

	this->mt = std::mt19937(rd());
}

bool segment::is_this_segment(int delay)
{
	if (delay >= this->lower_bound && delay <= this->upper_bound)
		return true;

	return false;
}

void segment::initialize_mean_sd()
{
	this->mean = std::accumulate(this->react_delays.begin(), this->react_delays.end(), 0.0) / this->react_delays.size();

	double sq_sum = std::inner_product(this->react_delays.begin(), this->react_delays.end(), this->react_delays.begin(), 0.0);

	this->standard_deviation = std::sqrt(sq_sum / this->react_delays.size() - this->mean * this->mean);

	printf("For %d - %d, mean is %f and sd is %f\n", this->lower_bound, this->upper_bound, this->mean, this->standard_deviation);
}

int segment::select_next_segment()
{
	std::discrete_distribution<int> d(this->next_segment_weight.begin(), this->next_segment_weight.end());

	return d(this->mt);
}

int segment::get_random_reaction_delay()
{
	if (!isnan(this->mean) && !isnan(this->standard_deviation))
	{
		std::normal_distribution<double>d(this->mean, this->standard_deviation); return static_cast<int>(d(this->mt));
	}
	else
	{
		std::uniform_int_distribution<int>d(this->lower_bound, this->upper_bound); return static_cast<int>(d(this->mt));
	}
}
