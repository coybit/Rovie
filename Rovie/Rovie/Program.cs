using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Xml.XPath;
using System.Xml;

namespace IMDBRates
{
    class Program
    {
        static void Main(string[] args)
        {
            List<KeyValuePair<string, double>> movies = new List<KeyValuePair<string, double>>();

            while (true)
            {
                string movieTitle = Console.ReadLine();
                double rate = GetRate(movieTitle);

                if (movieTitle == "!")
                {
                    break;
                }

                if (rate != -1)
                {
                    movies.Add(new KeyValuePair<string, double>(movieTitle, rate));
                    Console.WriteLine(rate);
                }
                else
                {
                    Console.WriteLine("Not Found!");
                }
            }

            movies.Sort(new Comparison<KeyValuePair<string, double>>((x, y) =>
            {
                return x.Value > y.Value ? -1 : +1;
            }));

            foreach (KeyValuePair<string, double> movie in movies)
            {
                Console.WriteLine(movie.Key + ": " + movie.Value);
            }
        }

        static double GetRate(string movieTitle)
        {
            string rate = null;
            string url = string.Format("http://imdbapi.org/?title={0}&type=xml&plot=simple&episode=1&limit=1&yg=0&mt=none&lang=en-US&offset=&aka=simple&release=simple&business=0&tech=0",
                movieTitle);
            System.Net.WebClient wc = new System.Net.WebClient();
            string response = wc.DownloadString(url);

            XmlDocument inventory = new XmlDocument();
            inventory.Load(url);

            XmlNodeList elements = inventory.SelectNodes("/IMDBDocumentList/item/rating");
            foreach (XmlElement element in elements)
            {
                rate = element.InnerText;
            }

            return (rate == null ? -1 : Convert.ToDouble(rate));
        }
    }
}
