#pragma once

struct segment
{
	int upper_bound;
	int lower_bound;

	double mean;
	double standard_deviation;

	// A list of float between 0 and 100 EXACTLY that will determine which segment we will choose next
	// This list is unique for every segment because each segment is USUALLY followed by a different series of segments
	std::vector<int> next_segment_weight;

	// all the points that fall into this segment's range
	std::vector<int> react_delays;

public:
	segment(int lower_bound, int upper_bound);

	bool is_this_segment(int delay);

	void initialize_mean_sd();

	// get next segment that we will pick from
	int select_next_segment();

	// use the upper/lower bound and mean/SD to generate a "random" reaction delay
	// NOTE: How to deal with lack of data point in a segment!
	int get_random_reaction_delay();

	std::mt19937 mt;
};


class humanizer
{
public:
	//ctor - take in user's mouse data file.bin
	humanizer(const char* dataFile);
	~humanizer();

	// Get the next time (in ms) that the bot should do something
	int   get_next_reaction_delay();

private:
	// just read the mouse clicks into react_delay_points
	bool  populate_delay_points();

	// Fill in our segments vector with the appropriate segments
	// Going to hardcode upper limit to 819200 aka 13.6 minutes
	bool  populate_segments();

	// initialize probability of the next "category" based on the current category
	bool  populate_segments_probability();

	// Generate an increasing fatigue multiplier to emulate slower reaction speed over time
	double get_fatigue_multiplier();

	// read the binary file
	bool read_biometric_file(const char* file);

	// list of all segments for this particular human data
	std::vector<segment> segments;

	// list of all react delay data
	std::vector<int> react_delay_points;

	// mouse replay ptr
	mouse_replay* mouse_data;

	std::mt19937 mt;

	int last_delay = 0;

	int next_segment_index = -1;
};


inline int humanizer::get_next_reaction_delay()
{
	// if this is our first roll, select one from the 200 to 400 band.
	if (next_segment_index == -1)
		this->next_segment_index = 1;

	this->last_delay = this->segments[next_segment_index].get_random_reaction_delay();
	this->next_segment_index = this->segments[next_segment_index].select_next_segment();

	return this->last_delay;
}

inline bool humanizer::populate_segments()
{
	for (size_t i = 1; i < 30; i++)
	{
		// lower bound in a range of data 100-199, 200 - 399, 400 - 799, etc in a semi-quadratic fashion
		int lower_bound = static_cast<int>(std::nearbyint(100 * pow(2, i)));

		// upper bound calculated by getting the next lower bound - 1
		int upper_bound = static_cast<int>(std::nearbyint(100 * pow(2, i + 1))) - 1;

		// capping the max delay range
		if (upper_bound >= LARGEST_DELAY)
			break;

		this->segments.push_back(segment(lower_bound, upper_bound));
	}

	if (this->segments.size() == 0)
		return false;

	for (auto& point : this->react_delay_points)
	{
		// if the latest delay is bigger than 13.6 minutes, then cap it between 13.6 and 6.8~ minutes
		if (point >= LARGEST_DELAY)
		{
			printf("[+] - Found point larger than max upper limit number at %d -> ", point);
			std::uniform_int_distribution<int> dist(LARGEST_DELAY / 2, LARGEST_DELAY - 1); point = dist(this->mt);
			printf("%d\n", point);
		}

		// put the points in the appropriate segment
		for (auto& segment : this->segments)
		{
			if (point >= segment.lower_bound && point <= segment.upper_bound)
			{
				segment.react_delays.push_back(point); break;
			}
		}
	}


	for (auto& segment : this->segments)
		segment.initialize_mean_sd();

	return TRUE;
}

inline bool humanizer::populate_delay_points()
{
	for (size_t i = 0; i < this->mouse_data->header.elements; i++)
	{
		mouse_biometric point = this->mouse_data->points[i];

		if (point.wParm == VK_LBUTTON && point.uMsg == WM_LBUTTONDOWN)
			react_delay_points.push_back(point.delay);
	}

	if (!react_delay_points.size())
		return FALSE;

	printf("[+] - We read %d points in our mouse data.\n", react_delay_points.size());

	return TRUE;
}

inline bool humanizer::populate_segments_probability()
{
	for (auto& segment_ : this->segments)
	{
		for (size_t i = 0; i < this->segments.size(); i++)
		{
			segment_.next_segment_weight.push_back(0);
		}

		for (size_t i = 0; i < this->react_delay_points.size() - 1; i++)
		{
			int point = this->react_delay_points[i];

			// if point belongs to the current segment
			if (segment_.is_this_segment(point))
			{
				int next_point = this->react_delay_points[i + 1];

				// see what segment did the next point falls into
				for (int j = 0; j < this->segments.size(); j++)
				{
					segment temp_segment = this->segments[j];

					// add 1 to the segment point count that the next point falls into, this will be used later to calculate the percentage of the next point
					if (temp_segment.is_this_segment(next_point))
						segment_.next_segment_weight[j]++;
				}
			}
		}
	}


	for (auto& segment_ : this->segments)
	{
		printf("%d - %d : \t\t", segment_.lower_bound, segment_.upper_bound);

		for (auto& weight : segment_.next_segment_weight)
			printf("%d ", weight);

		printf("\n");
	}

	return true;
}

inline double humanizer::get_fatigue_multiplier()
{
	return 0.0f;
}

inline bool humanizer::read_biometric_file(const char* file)
{
	std::ifstream mouse_data_file(file, std::ios::binary | std::ios::ate);

	if (!mouse_data_file.fail())
	{
		size_t FileSize = static_cast<size_t>(mouse_data_file.tellg());

		if (!FileSize)
			return FALSE;

		char* biometric_data = new char[FileSize]();

		mouse_data_file.seekg(0, std::ios::beg);

		mouse_data_file.read(biometric_data, FileSize);

		mouse_data_file.close();

		this->mouse_data = reinterpret_cast<mouse_replay*>(biometric_data);

		return TRUE;
	}

	return FALSE;
}
