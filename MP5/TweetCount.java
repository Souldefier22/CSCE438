
import java.io.File;
import java.io.IOException;
import java.util.*;

import org.apache.hadoop.fs.Path;
import org.apache.hadoop.conf.*;
import org.apache.hadoop.io.*;
import org.apache.hadoop.mapred.*;
import org.apache.hadoop.util.*;

public class TweetCount {
	
	public static class Map extends MapReduceBase implements Mapper<LongWritable, Text, Text, IntWritable> {
		private final static IntWritable one = new IntWritable(1);
		private Text hour = new Text();

		public void map(LongWritable key, Text value, OutputCollector<Text, IntWritable> output, Reporter reporter) throws IOException {
			String line = value.toString();
			//looking for if the line includes a T at the beginning to indicate the time of the tweet
			if(!line.isEmpty()){
				char identifier = line.charAt(0);
				if(identifier == 'T'){
					//breaks down line with time into the hour
					String[] words = line.split("\\s");
					String[] time = words[2].split(":");
					hour.set(time[0]);
					output.collect(hour, one); //using the hour as the key
				}
			}
		}
	}

	public static class Reduce extends MapReduceBase implements Reducer<Text, IntWritable, Text, IntWritable> {
		public void reduce(Text key, Iterator<IntWritable> values, OutputCollector<Text, IntWritable> output, Reporter reporter) throws IOException {
			int total = 0;
			//add up all the values that share a key
			while(values.hasNext()){
				IntWritable cur_value = values.next();
				total += cur_value.get();
			}
			output.collect(key, new IntWritable(total));
		}
	}

	public static void main(String[] args) throws Exception {
		JobConf conf = new JobConf(TweetCount.class);
		
		conf.setJobName("tweetcount");
		conf.setOutputKeyClass(Text.class);
		conf.setOutputValueClass(IntWritable.class);
		conf.setMapperClass(Map.class);
		conf.setCombinerClass(Reduce.class);
		conf.setReducerClass(Reduce.class);

		conf.setInputFormat(TextInputFormat.class);
		conf.setOutputFormat(TextOutputFormat.class);
		FileInputFormat.setInputPaths(conf, new Path(args[0]));
		FileOutputFormat.setOutputPath(conf, new Path(args[1]));

		JobClient.runJob(conf);
	}
}