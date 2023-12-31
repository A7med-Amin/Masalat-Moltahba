#include "world.hpp"
#include <iostream>

using namespace std;

namespace our
{

    // This is the upper limit on the number of entities in the world
    const uint64_t ENTITIES_UPPER_LIMIT = 55;
    const uint8_t SLICE_SIZE = 13;

    int generateRandomNumber(int min, int max)
    {
        // Generate a random number between min and max (inclusive)
        int randomNumber = rand() % (max - min + 1) + min;
        return randomNumber;
    }

    // This will deserialize a json array of entities and add the new entities to the current world
    // If parent pointer is not null, the new entities will be have their parent set to that given pointer
    // If any of the entities has children, this function will be called recursively for these children
    void World::deserialize(const nlohmann::json &data, Entity *parent)
    {
        std::vector<std::vector<bool>> entityMap(ENTITIES_UPPER_LIMIT, std::vector<bool>(3, false));

        if (!data.is_array())
            return;
        for (const auto &entityData : data)
        {
            cout << "New Entity" << endl;
            // TODO: (Req 8) Create an entity, make its parent "parent" and call its deserialize with "entityData".
            Entity *myEntity = add();
            // if (parent != nullptr)
            myEntity->parent = parent;
            myEntity->deserialize(entityData);

            if (entityData.contains("children"))
            {
                // TODO: (Req 8) Recursively call this world's "deserialize" using the children data
                //  and the current entity as the parent
                this->deserialize(entityData["children"], myEntity);
            }
            if (entityData.contains("duplicates"))
            {
                // cout << "here\n";
                cout << "duplicates" << endl;
                glm::vec4 duplicates = glm::vec4(entityData["duplicates"][0], entityData["duplicates"][1],
                                                 entityData["duplicates"][2], entityData["duplicates"][3]);
                for (int i = 1; i < (int)duplicates[0]; ++i)
                {
                    Entity *newDuplicateEntity = add();  // create a new entity using the add function in world.hpp
                    newDuplicateEntity->parent = parent; // set the parent of the new entity to the given parent
                    newDuplicateEntity->deserialize(
                        entityData); // deserialize the new entity using the given entityData
                    if ((bool)duplicates[2])
                    {
                        newDuplicateEntity->localTransform.position.x += float(i) * duplicates[1];
                        for (int j = 0; j < 3; ++j)
                        {
                            Entity *newDuplicateEntity = add();  // create a new entity using the add function in world.hpp
                            newDuplicateEntity->parent = parent; // set the parent of the new entity to the given parent
                            newDuplicateEntity->deserialize(
                                entityData); // deserialize the new entity using the given entityData
                            newDuplicateEntity->localTransform.position.z -= float(j) * 20;
                        }
                    }else
                    {
                        if ((bool)duplicates[3]){
                            int random = generateRandomNumber(0, 10);
                            newDuplicateEntity->localTransform.position.x +=  random;
                        }else{
                            newDuplicateEntity->localTransform.position.x += float(i) * duplicates[1];
                        }
                    }
                }
                deleteMarkedEntities();
            }
        }
    }

}