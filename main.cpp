
#include <HakariORM.h>
#include <iostream>

int main()
{
    // Create a repository object to interact with the database
    HakariORM::Repository repository("example.db");

    // Inserting an Image object into the database
    HakariORM::Image img;
    img.url = "https://example.com/image.jpg";
    img.image_height = 1080;
    img.image_width = 1920;

    // Creating a User object with an Image object
    HakariORM::User user;
    user.username = "example_user";
    user.age = 30;
    user.user_profile_img = img;

    // Inserting the User object into the database
    repository.insert(user);

    // Fetching a User object from the database
    HakariORM::User fetchedUser = repository.fetchBy<HakariORM::User>("username = 'example_user'");
    std::cout << "Fetched User: Username=" << fetchedUser.username << ", Age=" << fetchedUser.age << std::endl;
    std::cout << "Fetched User's Profile Image: URL=" << fetchedUser.user_profile_img.url << ", Height=" << fetchedUser.user_profile_img.image_height << ", Width=" << fetchedUser.user_profile_img.image_width << std::endl;

    return 0;
}