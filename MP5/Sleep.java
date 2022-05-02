import java.io.File;
import java.io.IOException;
import java.util.*;

import org.apache.hadoop.fs.Path;
import org.apache.hadoop.conf.*;
import org.apache.hadoop.io.*;
import org.apache.hadoop.mapred.*;
import org.apache.hadoop.util.*;

public class Sleep {
	
	public static class Map extends MapReduceBase implements Mapper<LongWritable, Text, Text, IntWritable> {
		private final static IntWritable one = new IntWritable(1);
		private Text hour = new Text();

		public void map(LongWritable key, Text value, OutputCollector<Text, IntWritable> output, Reporter reporter) throws IOException {
			String line = value.toString().toLowerCase();
			if(!line.isEmpty()){
				//looking for if the value includes sleep in the string at all
				if(line.indexOf("sleep") != -1){
					String[] words = line.split(" ");
					//breaks down line with time into the hour
					String[] time = words[1].split(":");
					hour.set(time[0]);
					output.collect(hour, one);
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
		JobConf conf = new JobConf(Sleep.class);
		conf.setJobName("sleep");
			
		conf.setOutputKeyClass(Text.class);
		conf.setOutputValueClass(IntWritable.class);
			
		conf.setMapperClass(Map.class);
		conf.setCombinerClass(Reduce.class);
		conf.setReducerClass(Reduce.class);

		conf.setInputFormat(TextInputFormat.class);
		//have the configuration sperate the tweets by tweets by finding the line where it beings with T
		conf.set("textinputformat.record.delimiter", "\n\nT");
		conf.setOutputFormat(TextOutputFormat.class);
		FileInputFormat.setInputPaths(conf, new Path(args[0]));
		FileOutputFormat.setOutputPath(conf, new Path(args[1]));

		JobClient.runJob(conf);
	}
}
