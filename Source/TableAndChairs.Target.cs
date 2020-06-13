using UnrealBuildTool;
using System.Collections.Generic;

public class TableAndChairsTarget : TargetRules
{
	public TableAndChairsTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		ExtraModuleNames.AddRange( new string[] { "TableAndChairs" } );
	}
}
