using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.NetworkInformation;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;
using System.Xml.Serialization;
using CLI;

namespace Sharp
{
    /*[Serializable] 
    public class TestComponent : Component 
    {
        public string name;
        public int number;
        public Vector3 position;
        public TestClass test;
    }*/

    public static class OpenVoxel
    {
        private static GlfwApplication AppInstance;

        private static void Main(string[] args)
        {
            AppInstance = new GlfwApplication();
            AppInstance.Initialize();

            OpenObject obj = AppInstance.CreateObject("Hello From Sharp");

            //OpenObject testOpenObject = CreateOpenObject("Hello From Sharp!");
            //OpenObject testOpenObject2 = testOpenObject;

            /* Here the target scene will be loaded and all the objects/components will be 
             * generated into the world. Right now test objects and components are being 
             * generated on the C++ side, but that will eventually change when the editor is 
             * developed a little more. */

            //SmartOpenObject newObjectTest = new SmartOpenObject("Hello From Sharp");
            //newObjectTest.obj.transform.position = new Vector3(0, 0, 0);

            /*TestComponent test = CreateComponent<TestComponent>(newObjectTest);
            test.name = "!";
            test.number = 420;
            test.position = new Vector3(4.0f, 2.0f, 8.0f);
            test.test = new TestClass { name2 = "?", test2 = new TestClass2 { name3 = "@" } };

            SerializeComponent(test);*/

            AppInstance.Start();
        }

        static Type[] nonManagedSerializableTypes = {
                typeof(Vector3)
        };

        public static string SerializeComponent<T>(T component)
        {
            List<object> serializableMembers = new List<object>();
            PropertyInfo[] properties = typeof(T).GetProperties();
            FieldInfo[] fields = typeof(T).GetFields();

            foreach (var property in properties) 
            {
                object value = property.GetValue(component);
                if (CanSerializeMember(value))
                    serializableMembers.Add(value);
            }

            foreach (var field in fields)
            {
                object value = field.GetValue(component);
                if (CanSerializeMember(value))
                    serializableMembers.Add(value);
            }

            foreach (var member in serializableMembers)
            {
                string serializedMember = ValueToString(member);
                Console.WriteLine(serializedMember);
            }

            return "";
        }

        private static bool CanSerializeMember(object value) 
        {
            if (value == null)
                return false;

            Type type = value.GetType();
            return type.IsSerializable || nonManagedSerializableTypes.Contains(type);
        }

        private static string ValueToString(object value) 
        {
            XmlSerializer xmlSerializer = new XmlSerializer(value.GetType());
            using (StringWriter textWriter = new StringWriter())
            {
                xmlSerializer.Serialize(textWriter, value);
                return textWriter.ToString();
            }
        }

        /*public static T CreateComponent<T>(this OpenObject openObject) where T : Component, new()
        {
            T newComponent = new T();
            newComponent.openObject = openObject;
            newComponent.transform = openObject.obj.transform;
            newComponent.Awake();
            openObject.obj.AddComponent(newComponent);

            return newComponent;
        }*/
    }
}
