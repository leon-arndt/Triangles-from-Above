#include "oxygine-framework.h"
#include "Box2D/Box2D.h"
#include "Box2DDebugDraw.h"
#include <iostream>
#include <list>
#include <vector>

using namespace oxygine;
using namespace std;

//it is our resources
//in real project you would have more than one Resources declarations. It is important on mobile devices with limited memory and you would load/unload them
Resources gameResources;

//DECLARE_SMART is helper, it does forward declaration and declares intrusive_ptr typedef for your class
DECLARE_SMART(MainActor, spMainActor);


const float SCALE = 100.0f;
const int LEVELCOUNT = 4;

int currentLevel = 0;

//list<spTriangle> triangleList; //unknown size, valid template type?

//spTriangle triangleArray[256];
//std::vector<spTriangle> triangleVector;
//Triangle triangle(Triangle::no_init);

b2Vec2 convert(const Vector2& pos)
{
    return b2Vec2(pos.x / SCALE, pos.y / SCALE);
}

Vector2 convert(const b2Vec2& pos)
{
    return Vector2(pos.x * SCALE, pos.y * SCALE);
}


// Circles
DECLARE_SMART(Circle, spCircle);
class Circle : public Sprite
{
public:
    Circle(b2World* world, const Vector2& pos, float scale = 1)
    {
        setResAnim(gameResources.getResAnim("circle")); //set the sprice for the object
        setAnchor(Vector2(0.5f, 0.5f));
        setTouchChildrenEnabled(false);

        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = convert(pos);

        b2Body* body = world->CreateBody(&bodyDef);

        setUserData(body);

        setScale(scale);

        b2CircleShape shape; //could be where the shape is defined
        shape.m_radius = getWidth() / SCALE / 2 * scale;

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &shape;
        fixtureDef.density = 1.0f;
        fixtureDef.friction = 0.6f; //increased from 0.3f

        body->CreateFixture(&fixtureDef);
        body->SetUserData(this);
    }
};

//Triangles
DECLARE_SMART(Triangle, spTriangle);
class Triangle : public Sprite
{
	//struct no_init_t {};

public:
	//no_init_t no_init;
	//Triangle(no_init_t) {}
	Triangle(b2World* world, const Vector2& pos, float scale = 1)
	{
		bool alive = true;
		
		setResAnim(gameResources.getResAnim("triangle")); //set the sprice for the object
		setAnchor(Vector2(0.5f, 0.5f)); //was 0.5f, 0.5f
		setTouchChildrenEnabled(false);

		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position = convert(pos);

		b2Body* body = world->CreateBody(&bodyDef);

		setUserData(body);

		setScale(scale);

		//b2PolygonShape shape;
		//b2CircleShape shape; //could be where the shape is defined

		//b2FixtureDef fixtureDef; //again below
		b2PolygonShape shape;
		b2Vec2 vertices[3];

		//1, 1, is too large
		vertices[0].Set(0, -0.5);
		vertices[1].Set(0.55, 0.5); // made wider to be same-length
		vertices[2].Set(-0.55, 0.5); // made wider to be same-length
		shape.Set(vertices, 3);
		//fixtureDef.shape = &shape; //again below

		shape.m_radius = getWidth() / SCALE / 20 * scale; // /2, later / 8

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &shape;
		fixtureDef.density = 10.0f; //increased from 1f
		fixtureDef.friction = 15.0f; //increased from 0.3f

		//parenting?


		//Gravity
		body->SetGravityScale(0);

		body->CreateFixture(&fixtureDef);
		body->SetUserData(this);
	}
};


DECLARE_SMART(Static, spStatic);
class Static : public Box9Sprite
{
public:
    Static(b2World* world, const RectF& rc)
    {
        //setHorizontalMode(Box9Sprite::TILING_FULL);
        //setVerticalMode(Box9Sprite::TILING_FULL);
        setResAnim(gameResources.getResAnim("pen"));
        setSize(rc.getSize());
        setPosition(rc.getLeftTop());
        setAnchor(Vector2(0.5f, 0.5f));

        b2BodyDef groundBodyDef;
        groundBodyDef.position = convert(getPosition());

        b2Body* groundBody = world->CreateBody(&groundBodyDef);

        b2PolygonShape groundBox;
        b2Vec2 sz = convert(getSize() / 2);
        groundBox.SetAsBox(sz.x, sz.y);
        groundBody->CreateFixture(&groundBox, 0.0f);
    }
};

class MainActor: public Actor
{
public:
    b2World* _world;
    spBox2DDraw _debugDraw;
	spSprite background;

    MainActor(): _world(0)
    {
        /// BUTTONS AND UI

		setSize(getStage()->getSize());

		//debug button
        /*
		spButton btn = new Button;
        btn->setX(getWidth() - btn->getWidth() - 3);
        btn->setY(3);
        btn->attachTo(this);
        btn->addEventListener(TouchEvent::CLICK, CLOSURE(this, &MainActor::showHideDebug));
		*/

		//gravity button
		spButton gravButton = new Button;
		gravButton->setX(getWidth() - gravButton->getWidth() - 5);
		gravButton->setY(90);
		gravButton->setPriority(32000); //Ensures the button is drawn on top
		gravButton->attachTo(this);
		gravButton->addEventListener(TouchEvent::CLICK, CLOSURE(this, &MainActor::enableGravity));

		spTextField gravButtonText = new TextField();
		gravButtonText->attachTo(getStage());
		gravButtonText->setPosition(gravButton->getPosition() + Vector2(-90.0f, 0));
		gravButtonText->setFontSize(30);
		gravButtonText->setPriority(32000); //Ensures the text is drawn on top
		gravButtonText->setFont(gameResources.getResFont("slabo"));
		gravButtonText->setText("Gravity");


		//restart button
		spButton restartButton = new Button;
		restartButton->setX(getWidth() - restartButton->getWidth() - 5);
		restartButton->setY(160);
		restartButton->setPriority(32000); //Ensures the button is drawn on top
		restartButton->attachTo(this);
		restartButton->addEventListener(TouchEvent::CLICK, CLOSURE(this, &MainActor::restart));

		spTextField restartButtonText = new TextField();
		restartButtonText->attachTo(getStage());
		restartButtonText->setPosition(restartButton->getPosition() + Vector2(-90.0f, 0));
		restartButtonText->setFontSize(30);
		restartButtonText->setPriority(32000); //Ensures the text is drawn on top
		restartButtonText->setFont(gameResources.getResFont("slabo"));
		restartButtonText->setText("Restart");


		//next level button
		spButton nextLevelButton = new Button;
		nextLevelButton->setX(getWidth() - restartButton->getWidth() - 5);
		nextLevelButton->setY(230);
		nextLevelButton->setPriority(32000); //Ensures the button is drawn on top
		nextLevelButton->attachTo(this);
		nextLevelButton->addEventListener(TouchEvent::CLICK, CLOSURE(this, &MainActor::nextLevel));

		spTextField levelButtonText = new TextField();
		levelButtonText->attachTo(getStage());
		levelButtonText->setPosition(nextLevelButton->getPosition() + Vector2(-90.0f, 0));
		levelButtonText->setFontSize(30);
		levelButtonText->setPriority(32000); //Ensures the text is drawn on top
		levelButtonText->setFont(gameResources.getResFont("slabo"));
		levelButtonText->setText("Level");




		//Write some misc text on the screen

		//ResFont *font = gameResources.getResFont("Roboto-Black");
		spTextField text = new TextField();
		text->attachTo(getStage());
		text->setPosition(Vector2(480, 12.0f));
		//text->setFont(font);
		text->setFontSize(48);
		text->setHAlign(TextStyle::HALIGN_MIDDLE);
		text->setFont(gameResources.getResFont("slabo"));
		text->setText("Spawn Triangles by Clicking");


		//Event listener for clicking
        addEventListener(TouchEvent::CLICK, CLOSURE(this, &MainActor::click));


        _world = new b2World(b2Vec2(0, 10));


        spStatic ground = new Static(_world, RectF(getWidth() / 2, getHeight() - 10, getWidth() - 100, 30));
        addChild(ground);

		//Adds a pentagon at the beginning of the game?
		//spPentagon pentagon = new Pentagon(_world, getSize() / 2 + Vector2(0.0f, 92.0f), 1);
		//addChild(pentagon);

		//background moved to here
		background = new Sprite; //had a type before
		background->setResAnim(gameResources.getResAnim("level1"));
		background->setAnchor(Vector2(0.5f, 1.0f));
		background->setPosition(Vector2(480, 610.0f));
		addChild(background);
		//background->attachTo(actor);
    }

	//Update loop for the world
    void doUpdate(const UpdateState& us)
    {
        //in real project you should make steps with fixed dt, check box2d documentation
        _world->Step(us.dt / 1000.0f, 6, 2);

        //update each body position on display
        b2Body* body = _world->GetBodyList();
        while (body)
        {
            Actor* actor = (Actor*)body->GetUserData();
            b2Body* next = body->GetNext();
            if (actor)
            {
                const b2Vec2& pos = body->GetPosition();
                actor->setPosition(convert(pos));
                actor->setRotation(body->GetAngle());

                //remove fallen bodies (outside the game view)
                if (actor->getY() > getHeight() + 50)
                {
                    body->SetUserData(0);
                    _world->DestroyBody(body);

                    actor->detach();
                }
            }

            body = next;
        }
    }

	//Used for showing and hiding information about debug
    void showHideDebug(Event* event)
    {
        TouchEvent* te = safeCast<TouchEvent*>(event);
        te->stopsImmediatePropagation = true;
        if (_debugDraw)
        {
            _debugDraw->detach();
            _debugDraw = 0;
            return;
        }

        _debugDraw = new Box2DDraw;
        _debugDraw->SetFlags(b2Draw::e_shapeBit | b2Draw::e_jointBit | b2Draw::e_pairBit | b2Draw::e_centerOfMassBit);
        _debugDraw->attachTo(this);
        _debugDraw->setWorld(SCALE, _world);
        _debugDraw->setPriority(1);
    }

	//Enable gravity event
	void enableGravity(Event* event)
	{
		//Get a list of all physics objects		
		b2Body* body = _world->GetBodyList();
		
		while (body)
		{
			b2Body* next = body->GetNext();
			body->SetGravityScale(1);
			body->ApplyForceToCenter(b2Vec2(1, 1), true);

			//go to the next body in the list
			body = next;
		}

	}

	//delete all triangles function
	void deleteTriangles() {
		b2Body* body = _world->GetBodyList();

		while (body)
		{
			Actor* actor = (Actor*)body->GetUserData();
			b2Body* next = body->GetNext();
			if (actor)
			{
				//remove all triangles (restart the level)
				if (actor->getY() < getHeight() - 50)
				{
					//body->ApplyForceToCenter(b2Vec2(std::rand() * 10000.0f, std::rand() * -10000.0f), true);
					body->SetUserData(0);
					_world->DestroyBody(body);

					actor->detach();
				}
			}

			body = next;
		}
	}
	//restart event
	void restart(Event* event)
	{
		// delete all triangles
		deleteTriangles();
	}

	//next level event
	void nextLevel(Event* event)
	{
		// delete all triangles
		deleteTriangles();

		//loop through the levels
		currentLevel = (currentLevel + 1) % LEVELCOUNT;
		
		//simple switch case to show the correct level
		switch (currentLevel) {
		case 0:
			background->setResAnim(gameResources.getResAnim("level1"));
			break;
		case 1:
			background->setResAnim(gameResources.getResAnim("level2"));
			break;
		case 2:
			background->setResAnim(gameResources.getResAnim("level3"));
			break;
		case 3:
			background->setResAnim(gameResources.getResAnim("level4"));
			break;
		}
		
	}

    void click(Event* event)
    {
        TouchEvent* te = safeCast<TouchEvent*>(event);

		if (event->target.get() == this)
		{

			//spawn a triangle
			spTriangle triangle = new Triangle(_world, te->localPosition);
			//triangleArray.push_back(triangle); //adds the triangle to the end of the list			
			triangle->attachTo(this);
        }

        if (event->target->getUserData())
        {
            //shot to triangle: this deletes the triangle
			spActor actor = safeSpCast<Actor>(event->target);
			b2Body* body = (b2Body*)actor->getUserData();

			body->SetUserData(0);
			_world->DestroyBody(body);

			actor->detach();

            /*spActor actor = safeSpCast<Actor>(event->target);
            b2Body* body = (b2Body*)actor->getUserData();

            Vector2 dir = actor->getPosition() - te->localPosition;
            dir = dir / dir.length() * body->GetMass() * 200;

            body->ApplyForceToCenter(b2Vec2(dir.x, dir.y), true);

            spSprite sprite = new Sprite();
            sprite->setResAnim(gameResources.getResAnim("shot"));
            Vector2 local = actor->parent2local(te->localPosition);
            sprite->setPosition(local);
            sprite->setAnchor(Vector2(0.5f, 0.5f));
            sprite->attachTo(actor);*/
        }
    }
};

//MOVED TRIANGLE TO HERE




void example_preinit()
{
}

void example_init()
{
    //load xml file with resources definition
    gameResources.loadXML("res.xml");

    //lets create our client code simple actor
    //prefix 'sp' here means it is intrusive Smart Pointer
    //it would be deleted automatically when you lost ref to it
    spMainActor actor = new MainActor;
    //and add it to Stage as child
    getStage()->addChild(actor);


	//background moved from here
	//spSprite background = new Sprite;
	//background->setResAnim(gameResources.getResAnim("level1"));
	//background->setAnchor(Vector2(0.5f, 1.0f));
	//background->setPosition(Vector2(480, 610.0f));
	//background->attachTo(actor);
}

void example_destroy()
{
    gameResources.free();
}

void example_update()
{

}