
namespace StubClass
{
    public class Static
    {
        public static Static Instance()
        {
            return new Static();
        }

        public static int GetInt()
        {
            return 574716;
        }

        public static string GetString()
        {
            return "static";
        }

        public string InstanceString()
        {
            return "instance." + this.GetHashCode();
        }
    }
}
