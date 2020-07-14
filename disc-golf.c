settings
{
  main
    {
    Description: "Disc Golf in Overwatch!  Place a tee and a hole, spawn in your disc, pick it up, and throw it!  Use angles to make it curve like a real disc!  (Early alpha version by nobody#1774.  Latest version at: https://workshop.codes/disc-golf)"
       }

  lobby
    {
      Allow Players Who Are In Queue: Yes
        Max FFA Players: 6
        Max Spectators: 12
	}

  modes
    {
      Deathmatch
        {
          Game Length In Minutes: 15
            Score To Win: 50

            enabled maps
          {
            Blizzard World
              Blizzard World Winter
              }
        }

      disabled Team Deathmatch
      {
        Game Length In Minutes: 15
          }

      General
        {
          Game Mode Start: Manual
            Limit Roles: 2 Of Each Role Per Team
            }
    }

  heroes
    {
      General
        {
          Damage Dealt: 10%
            Damage Received: 10%
            Primary Fire: Off
            Ultimate Generation: 10%
            Ultimate Generation - Combat: 0%
            Ultimate Generation - Passive: 0%

            Symmetra
          {
            Sentry Turret: Off
              }

          Torbjörn
            {
              Deploy Turret: Off
                }

          Wrecking Ball
          {
            Movement Gravity: 50%
              }

          disabled heroes
          {
            Wrecking Ball
              }
        }
    }
}

variables
{
 global:
 6: HUD_Instructions
   25: INT_Loop_Index

   player:
   0: INT_Curve_Angle
   1: INT_Last_Facing_Angle
   2: INT_Turn_Rate
   3: BOOL_Disc_Thrown
   4: ENTITY_Disc
   5: EFFECTS_Disc
   7: BOOL_Holding_Disc
   8: VECTOR_Disc_Curve_Impulse
   9: INT_Disc_Curve_Speed
   10: HUD_Holding_Disc
   20: EFFECT_Goal_Name
   21: EFFECT_Tee_Name
   22: EFFECT_Tee
   23: POINT_Tee
   24: EFFECT_Goal
   25: POINT_Goal
   }

subroutines
{
 0: Disc_Clear
   1: Disc_Spawn
   }

rule("PLAYER: Spawn disc")
{
  event
    {
      Ongoing - Each Player;
      All;
      All;
    }

  conditions
    {
      Is Button Held(Event Player, Interact) == True;
      Is Button Held(Event Player, Primary Fire) == True;
    }

  actions
    {
      Call Subroutine(Disc_Clear);
      Call Subroutine(Disc_Spawn);
    }
}

rule("SUBR: Disc_Spawn (for player in variable Global.PLAYER_Spawning_Disc)")
{
  event
    {
      Subroutine;
      Disc_Spawn;
    }

  actions
    {
      "Create dummy bot for disc at player's position."
        Create Dummy Bot(Hero(Wrecking Ball), All Teams, -1, Position Of(Event Player), Vector(0, 0, 0));
      "Save bot to player's variable."
        Event Player.ENTITY_Disc = Last Created Entity;
      "Make bot invisible and phased out."
        Set Invisible(Event Player.ENTITY_Disc, All);
      Set Status(Event Player.ENTITY_Disc, Null, Phased Out, 9999);
      Create Effect(All Players(All Teams), Sphere, White, Position Of(Event Player.ENTITY_Disc), 0.500, Visible To Position and Radius);
      Modify Player Variable(Event Player.ENTITY_Disc, EFFECTS_Disc, Append To Array, Last Created Entity);
      Create Effect(All Players(All Teams), Sphere, Red, Position Of(Event Player.ENTITY_Disc), 0.250, Visible To Position and Radius);
      Modify Player Variable(Event Player.ENTITY_Disc, EFFECTS_Disc, Append To Array, Last Created Entity);
      Create Effect(All Players(All Teams), Sphere, Blue, Position Of(Event Player.ENTITY_Disc), 0.750, Visible To Position and Radius);
      Modify Player Variable(Event Player.ENTITY_Disc, EFFECTS_Disc, Append To Array, Last Created Entity);
      Create In-World Text(All Players(All Teams), Event Player, Position Of(Event Player.ENTITY_Disc), 0.750, Clip Against Surfaces,
                           Visible To Position and String, White, Default Visibility);
      Modify Player Variable(Event Player.ENTITY_Disc, EFFECTS_Disc, Append To Array, Last Created Entity);
    }
}

rule("SUBR: Disc_Clear (for player in variable Global.PLAYER_Spawning_Disc)")
{
  event
    {
      Subroutine;
      Disc_Clear;
    }

  actions
    {
      "Destroy dummy bot for player's disc."
        Destroy Dummy Bot(All Teams, Slot Of(Event Player.ENTITY_Disc));
      "Destroy effects for player's disc."
        For Global Variable(INT_Loop_Index, 0, Count Of(Event Player.ENTITY_Disc.EFFECTS_Disc), 1);
      Destroy Effect(Event Player.ENTITY_Disc.EFFECTS_Disc[Global.INT_Loop_Index]);
      Destroy Icon(Event Player.ENTITY_Disc.EFFECTS_Disc[Global.INT_Loop_Index]);
      Destroy In-World Text(Event Player.ENTITY_Disc.EFFECTS_Disc[Global.INT_Loop_Index]);
      End;
      "Destroy effects for player."
        For Global Variable(INT_Loop_Index, 0, Count Of(Event Player.EFFECTS_Disc), 1);
      Destroy Effect(Event Player.EFFECTS_Disc[Global.INT_Loop_Index]);
      Destroy Icon(Event Player.EFFECTS_Disc[Global.INT_Loop_Index]);
      End;
      "Set player disc variables to 0."
        Event Player.ENTITY_Disc = 0;
      Event Player.EFFECTS_Disc = 0;
      Destroy HUD Text(Event Player.HUD_Holding_Disc);
    }
}

rule("PLAYER: Throw charge meter (ult meter)")
{
  event
    {
      Ongoing - Each Player;
      All;
      All;
    }

  conditions
    {
      Is Alive(Event Player) == True;
      Ultimate Charge Percent(Event Player) != Event Player.INT_Curve_Angle;
    }

  actions
    {
      Set Ultimate Charge(Event Player, Event Player.INT_Curve_Angle);
    }
}

rule("PLAYER: Set throw charge (reload)")
{
  event
    {
      Ongoing - Each Player;
      All;
      All;
    }

  conditions
    {
      Is Button Held(Event Player, Reload) == True;
    }

  actions
    {
      If(Event Player.INT_Curve_Angle < 50);
      Event Player.INT_Curve_Angle += 1;
      Else;
      Event Player.INT_Curve_Angle = 0;
      Set Ultimate Charge(Event Player, 0);
      End;
      Wait(0.250, Ignore Condition);
      Loop If Condition Is True;
    }
}

rule("PLAYER: Pick up disc")
{
  event
    {
      Ongoing - Each Player;
      All;
      All;
    }

  conditions
    {
      Event Player.ENTITY_Disc != 0;
      Distance Between(Event Player.ENTITY_Disc, Event Player) <= 1.500;
      Is Button Held(Event Player, Crouch) == True;
    }

  actions
    {
      Call Subroutine(Disc_Clear);
      Create Effect(All Players(All Teams), Ring, Green, Event Player, 1, Visible To Position and Radius);
      Modify Player Variable(Event Player, EFFECTS_Disc, Append To Array, Last Created Entity);
      Event Player.BOOL_Holding_Disc = True;
      Set Status(Event Player, Null, Rooted, 9999);
      Create HUD Text(Event Player, Custom String("Holding disc"), Null, Custom String(
                                                                                       "To throw, press primary fire while slowly turning"), Top, 0, White, White, White, Visible To and String, Default Visibility);
      Event Player.HUD_Holding_Disc = Last Text ID;
      "Reset angle."
        disabled Event Player.INT_Curve_Angle = 25;
    }
}

rule("PLAYER: Throw disc")
{
  event
    {
      Ongoing - Each Player;
      All;
      All;
    }

  conditions
    {
      Event Player.BOOL_Holding_Disc == True;
      Is Button Held(Event Player, Primary Fire) == True;
    }

  actions
    {
      "Clear player-holding-disc effects."
        Call Subroutine(Disc_Clear);
      Call Subroutine(Disc_Spawn);
      "Calcuate and set disc curve vector and speed."
        Event Player.ENTITY_Disc.VECTOR_Disc_Curve_Impulse = Event Player.INT_Curve_Angle - 25 < 0 ? Vector(1, 0, 0) : Vector(-1, 0, 0);
      If(Event Player.INT_Curve_Angle <= 25);
      Event Player.ENTITY_Disc.INT_Disc_Curve_Speed = Absolute Value(Event Player.INT_Curve_Angle - 25) / 25;
      Else If(Event Player.INT_Curve_Angle == 25);
      Event Player.ENTITY_Disc.INT_Disc_Curve_Speed = 0;
      Else;
      Event Player.ENTITY_Disc.INT_Disc_Curve_Speed = (Event Player.INT_Curve_Angle - 25) / 25;
      End;
      "Scale curve speed."
        disabled Event Player.ENTITY_Disc.INT_Disc_Curve_Speed = 1.500;
      "Scale curve speed by throw speed (multiply by the fraction throw speed is of 50)."
        disabled Event Player.ENTITY_Disc.INT_Disc_Curve_Speed = Event Player.ENTITY_Disc.INT_Disc_Curve_Speed * (Min(50, Absolute Value(
                                                                                                                                         Event Player.INT_Turn_Rate)) / 50);
      "Wait for dummy to spawn in.  (Maybe we could just phase it out until after it's thrown instead.)"
        Wait(0.150, Ignore Condition);
      Set Facing(Event Player.ENTITY_Disc, Facing Direction Of(Event Player), To World);
      "Apply impulse to disc (uncapped speed)."
        disabled Apply Impulse(Event Player.ENTITY_Disc, Facing Direction Of(Event Player), Absolute Value(Event Player.INT_Turn_Rate) * 2,
                               To World, Incorporate Contrary Motion);
      "Apply impulse to disc (capped to 40)."
        Apply Impulse(Event Player.ENTITY_Disc, Facing Direction Of(Event Player), Min(40, Absolute Value(Event Player.INT_Turn_Rate) * 2),
                      To World, Incorporate Contrary Motion);
      Event Player.ENTITY_Disc.BOOL_Disc_Thrown = True;
      Event Player.BOOL_Holding_Disc = False;
      Clear Status(Event Player, Rooted);
      Destroy HUD Text(Event Player.HUD_Holding_Disc);
    }
}

rule("PLAYER: Disable buttons")
{
  event
    {
      Ongoing - Each Player;
      All;
      All;
    }

  conditions
    {
      Is Alive(Event Player) == True;
    }

  actions
    {
      Disallow Button(Event Player, Ultimate);
    }
}

rule("DISC: Apply sideways motion in air")
{
  event
    {
      Ongoing - Each Player;
      All;
      All;
    }

  conditions
    {
      Hero Of(Event Player) == Hero(Wrecking Ball);
      Is In Air(Event Player) == True;
      Event Player.BOOL_Disc_Thrown == True;
    }

  actions
    {
      disabled Small Message(All Players(All Teams), Custom String("applying impulse: {0}", Event Player.VECTOR_Disc_Curve_Impulse));
      Apply Impulse(Event Player, Event Player.VECTOR_Disc_Curve_Impulse, Event Player.INT_Disc_Curve_Speed, To Player,
                    Incorporate Contrary Motion);
      Wait(0.100, Ignore Condition);
      Loop If Condition Is True;
    }
}

rule("INIT")
{
  event
    {
      Ongoing - Each Player;
      All;
      All;
    }

  conditions
    {
      Has Spawned(Event Player) == True;
    }

  actions
    {
      Event Player.INT_Curve_Angle = 25;
    }
}

rule("PLAYER: Record turn rate")
{
  event
    {
      Ongoing - Each Player;
      All;
      All;
    }

  conditions
    {
      Event Player.BOOL_Holding_Disc == True;
    }

  actions
    {
      Event Player.INT_Turn_Rate = Angle Difference(Event Player.INT_Last_Facing_Angle, Horizontal Facing Angle Of(Event Player));
      Event Player.INT_Last_Facing_Angle = Horizontal Facing Angle Of(Event Player);
      Wait(0.100, Ignore Condition);
      Loop If Condition Is True;
    }
}

rule("DISC: Cancel motion on ground")
{
  event
    {
      Ongoing - Each Player;
      All;
      All;
    }

  conditions
    {
      Hero Of(Event Player) == Hero(Wrecking Ball);
      Is In Air(Event Player) == False;
    }

  actions
    {
      Event Player.BOOL_Disc_Thrown = False;
    }
}

rule("DEBUG: Destroy all bots")
{
  event
    {
      Ongoing - Each Player;
      All;
      All;
    }

  conditions
    {
      Event Player == Host Player;
      Is Button Held(Event Player, Interact) == True;
      Is Button Held(Event Player, Reload) == True;
    }

  actions
    {
      Destroy All Dummy Bots;
      Destroy All Effects;
      Destroy All Icons;
    }
}

disabled rule("DEBUG: HUD")
{
  event
    {
      Player Joined Match;
      All;
      All;
    }

  conditions
    {
      Hero Of(Event Player) != Hero(Wrecking Ball);
    }

  actions
    {
      Create HUD Text(Event Player, Custom String("Turn rate: {0}  Disc vector: {1}  Curve impulse: {2}", Event Player.INT_Turn_Rate,
                                                  Event Player.ENTITY_Disc.VECTOR_Disc_Curve_Impulse, Event Player.ENTITY_Disc.INT_Disc_Curve_Speed), Null, Null, Left, 0, White,
                      White, White, Visible To and String, Default Visibility);
    }
}

rule("DEBUG: Create goals")
{
  event
    {
      Ongoing - Each Player;
      All;
      All;
    }

  conditions
    {
      Is Button Held(Event Player, Interact) == True;
      Is Button Held(Event Player, Crouch) == True;
    }

  actions
    {
      Destroy Effect(Event Player.EFFECT_Goal);
      Event Player.POINT_Goal = Position Of(Event Player);
      Create Effect(All Players(All Teams), Sphere, Red, Event Player.POINT_Goal, 2, Visible To Position and Radius);
      Event Player.EFFECT_Goal = Last Created Entity;
      Destroy Effect(Event Player.EFFECT_Goal_Name);
      Create In-World Text(All Players(All Teams), Event Player, Position Of(Event Player.POINT_Goal), 0.750, Do Not Clip,
                           Visible To Position and String, Red, Default Visibility);
      Event Player.EFFECT_Goal_Name = Last Created Entity;
    }
}

disabled rule("SCORE (not working, will be easier with actual designed courses)")
{
  event
    {
      Ongoing - Each Player;
      All;
      All;
    }

  conditions
    {
      Hero Of(Event Player) == Hero(Wrecking Ball);
      Is In Air(Event Player) == False;
      Speed Of(Event Player) == 0;
      Is Alive(Event Player) == True;
      Is True For Any(Filtered Array(All Players(All Teams), !Is Dummy Bot(Current Array Element)),
                      Current Array Element.POINT_Goal != 0 && Distance Between(Current Array Element.EFFECT_Goal, Position Of(Event Player)) <= 2)
        == True;
    }

  actions
    {
      Big Message(All Players(All Teams), Custom String("{0} Scored!", First Of(Filtered Array(All Players(All Teams),
                                                                                               Event Player == Current Array Element.ENTITY_Disc))));
    }
}

rule("HUD: Instructions")
{
  event
    {
      Ongoing - Global;
    }

  conditions
    {
      Global.HUD_Instructions == 0;
    }

  actions
    {
      Create HUD Text(All Players(All Teams), Custom String("Place goal"), Custom String(""), Custom String("Interact + Crouch"), Right,
                      0, Red, White, White, Visible To and String, Default Visibility);
      Create HUD Text(All Players(All Teams), Custom String("Place tee"), Custom String(""), Custom String("Interact + Jump"), Right, 0,
                      Green, White, White, Visible To and String, Default Visibility);
      Create HUD Text(All Players(All Teams), Custom String("Angle"), Custom String("Ultimate charge 0-25=left, 26-50=right"),
                      Custom String("Reload"), Right, 0, Orange, Orange, White, Visible To and String, Default Visibility);
      Create HUD Text(All Players(All Teams), Custom String("Throw disc"), Custom String(" while turning (slowly!)"), Custom String(
                                                                                                                                    "Primary Fire"), Right, 0, White, Turquoise, White, Visible To and String, Default Visibility);
      Create HUD Text(All Players(All Teams), Custom String("Pick up disc"), Custom String("While next to disc"), Custom String(
                                                                                                                                "Crouch"), Right, 0, White, Turquoise, White, Visible To and String, Default Visibility);
      Create HUD Text(All Players(All Teams), Custom String("Spawn disc"), Custom String("Can only interact with own disc"),
                      Custom String("Interact + Primary Fire"), Right, 0, White, Turquoise, White, Visible To and String, Default Visibility);
      Create HUD Text(All Players(All Teams), Custom String("Share code"), Custom String("https://workshop.codes/disc-golf"),
                      Custom String("V337X"), Left, 0, White, White, White, Visible To and String, Default Visibility);
	}
}

rule("DEBUG: Create tees")
{
	event
	{
		Ongoing - Each Player;
		All;
		All;
	}

	conditions
	{
		Is Button Held(Event Player, Jump) == True;
		Is Button Held(Event Player, Interact) == True;
	}

	actions
	{
		Destroy Effect(Event Player.EFFECT_Tee);
		Event Player.POINT_Tee = Position Of(Event Player);
		Create Effect(All Players(All Teams), Sphere, Green, Event Player.POINT_Tee, 0.250, Visible To Position and Radius);
		Event Player.EFFECT_Tee = Last Created Entity;
		Destroy Effect(Event Player.EFFECT_Tee_Name);
		Create In-World Text(All Players(All Teams), Event Player, Event Player.POINT_Tee, 0.750, Do Not Clip,
			Visible To Position and String, Green, Default Visibility);
		Event Player.EFFECT_Tee_Name = Last Created Entity;
	}
}

rule("PLAYER: Leaves match")
{
	event
	{
		Player Left Match;
		All;
		All;
	}

	actions
	{
		disabled Small Message(Host Player, Custom String("Player left match: {0}", Event Player));
		Call Subroutine(Disc_Clear);
	}
}
