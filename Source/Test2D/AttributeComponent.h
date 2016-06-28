// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "PlayerAttributes.h"
#include "Components/ActorComponent.h"
#include "DamageTextActor.h"
#include "AbstractSkillData.h"
#include "Status.h"
//#include <map>
#include "AttributeComponent.generated.h"
class UAbstractSkill;
class AEquippable;
//class AHpBarActor;



DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDamageTakenDelegate, float, damageTaken, float, damageBeforeDefense, UAttributeComponent *, damageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDamageEvadeDelegate, float, damageTaken, float, damageBeforeDefense, UAttributeComponent *, damageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSkillHitDelegate, UAttributeComponent *, hitAttribComp, int, skillUseID);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRespawnDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHUDEventDelegate);



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TEST2D_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()
private:
	float currRespawnTime;
	UPROPERTY(Replicated)
		bool Alive;

	UPROPERTY(Replicated)
		int skillCounter = 0;

	typedef TMap<UAttributeComponent *, TArray<ATest2DAlly*>> TAttribMap;
	typedef TMap<TSubclassOf<ATest2DAlly>, TAttribMap> TStatusMap;

	//typedef std::map<UAttributeComponent *, TArray<ATest2DAlly*>> attribMap;
	//typedef std::map<TSubclassOf<ATest2DAlly>, attribMap> statusMap;

	//UPROPERTY(Replicated)
	TStatusMap StatusMap;

	UPROPERTY(Replicated)
		TArray<TSubclassOf<ATest2DAlly>> AppliedStatuses;
public:
	// Sets default values for this component's properties
	UAttributeComponent();

	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	//	TSubclassOf<AHpBarActor> HpBarClass;
	//UPROPERTY(BlueprintReadOnly, Category = Attributes)
	//	AHpBarActor * HpBar;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		void Init();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = Attributes)
		void SpawnDamageText(TSubclassOf<ADamageTextActor> damageTextActorStyle, float finalDmg, EDmgType DmgTextType, bool criticalHit, int MaxHp, UAttributeComponent * DamageCauser, int DmgTextSpawnID);

	void getKnockedBack(FVector knockBack);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
		TSubclassOf<ADamageTextActor> damageTextActorStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Attributes)
		EAllianceType AllianceType = EAllianceType::AT_Player;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Attributes)
		bool bPvpEnabled = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Attributes)
		FPlayerAttributes baseStats;
	UPROPERTY(BlueprintReadOnly, Replicated, Category = Attributes)
		FPlayerAttributes totalStats;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = Attributes)
		bool bIsInvuln;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = Attributes)
		bool bIsEvading;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = Attributes)
		float hpRegenTimer;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = Attributes)
		int currHp;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Attributes)
		bool isAlive() { return Alive; }

	UPROPERTY(BlueprintReadOnly, Replicated, Category = Attributes)
		int currJumps;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
		float MaxRespawnTime;

	UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = Attributes)
		virtual void TakeDamage(
			UAttributeComponent * DamageCauser,
			UObject * DamageSource,
			int SkillUseID,
			EDmgType DmgType,
			FAbstractSkillData otherSkillData,
			FPlayerAttributes otherAttributes,
			FVector locationOfCollision,
			TSubclassOf<ADamageTextActor> dmgTextActor);

	UPROPERTY(Replicated, BlueprintAssignable, Category = "Test")
		FRespawnDelegate onRespawnDelegate;

	UPROPERTY(Replicated, BlueprintAssignable, Category = "Test")
		FSkillHitDelegate onSkillHitDelegate;

	UPROPERTY(Replicated, BlueprintAssignable, Category = "Test")
		FDamageTakenDelegate onTakeDamageDelegate;

	UPROPERTY(Replicated, BlueprintAssignable, Category = "Test")
		FDamageEvadeDelegate onEvadeDamageDelegate;

	UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = Attributes)
		virtual void Heal(float HealAmount);

	UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = Attributes)
		virtual void TogglePVP();

	UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = Attributes)
		virtual void SetEvade(bool isEvade);

	//UFUNCTION(NetMulticast, WithValidation, Reliable, BlueprintCallable, Category = Attributes)
	//	virtual void NetMulticastRPC_TogglePVP(bool pvp);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = Attributes)
		virtual void Die();

	UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = Attributes)
		virtual void Respawn();

	UFUNCTION(BlueprintCallable, Category = Attributes)
		virtual bool canTarget(UAttributeComponent * other);


	UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = Attributes)
		void RecalculateTotalStats();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
		TArray<FStatusApplicationData> SelfStatusApplication_OnHit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
		TArray<FStatusApplicationData> EnemyStatusApplication_OnHit;

	UFUNCTION(BlueprintCallable, Category = "Status")
		TArray<ATest2DAlly *> applyStatus(TArray<FStatusApplicationData> statusToApply, UAttributeComponent * appliedBy);

	ATest2DAlly * spawnStatus(UAttributeComponent * appliedTo, FStatusApplicationData allySpawnData);

	UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = "Status")
		void removeStatus(ATest2DAlly * allyToRemove);

	UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = "Status")
		void clearStatuses();

	UFUNCTION(BlueprintCallable, Category = "Status")
		UTexture2D * getStatusIcon(TSubclassOf<ATest2DAlly> statusClass);

	UFUNCTION(BlueprintCallable, Category = "Status")
		int getTotalStacksFromStatus(TSubclassOf<ATest2DAlly> statusClass);

	UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = "Status")
		void updateStatusBarStatuses(bool checkForShowOnStatusBar);

	UFUNCTION(BlueprintCallable, Category = "Status")
		void updateStatusBar_RepNotify();

	UPROPERTY(ReplicatedUsing = updateStatusBar_RepNotify, BlueprintReadOnly, Category = Attributes)
		TArray<FStatusApplicationData> statusBarStatuses;

	UPROPERTY(Replicated, BlueprintAssignable, Category = "Test")
		FHUDEventDelegate onUpdateStatusBarDelegate;

	TMap<UAttributeComponent*, TArray< ADamageTextActor*>> DamageTextArr;

	UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = "DamageTextUse")
		void incrSkillUseID();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DamageTextUse")
		int getSkillUseID() { return skillCounter; }

	UPROPERTY(BlueprintReadWrite, Category = "Test")
		TArray<AEquippable*> EquippedItems;
};

