using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CLI;

namespace Sharp
{
    public static class OpenVoxel
    {
        private static void Main(string[] args)
        {
            GlfwApplication application = new GlfwApplication();
            application.Initialize();
            application.Start();
        }

        public static void CreateComponent<T>(this OpenObject openObject) where T : Component, new()
        {
            Component newComponent = new T();
            newComponent.openObject = openObject;
            newComponent.transform = openObject.transform;
            newComponent.Awake();
            openObject.AddComponent(newComponent);
        }
    }
}
