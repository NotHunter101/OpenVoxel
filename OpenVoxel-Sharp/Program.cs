using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CLI;

namespace Sharp
{
    internal class Program
    {
        class Test2 : ComponentTest 
        {
            public override void Test()
            {
                Console.WriteLine("BAR!");
            }
        }

        static void Main(string[] args)
        {
            GlfwApplication application = new GlfwApplication();

            Test2 test = new Test2();
            application.AddComponent(test);
            application.TestInheritance();

            application.Start();
            Console.ReadLine();
        }
    }
}
